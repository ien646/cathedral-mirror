#include <cathedral/engine/nodes/mesh3d_node.hpp>

#include <cathedral/engine/scene.hpp>

namespace cathedral::engine
{
    void mesh3d_node::set_mesh(const std::string& path)
    {
        _mesh_path = path;
        _needs_refresh_buffers = true;
    }

    void mesh3d_node::set_mesh(std::shared_ptr<mesh_buffer> mesh_buffer)
    {
        _mesh_buffers = std::move(mesh_buffer);
        _mesh_path = std::nullopt;
        _needs_refresh_buffers = false;
    }

    void mesh3d_node::set_material(std::optional<std::string> name)
    {
        if (!name.has_value())
        {
            _material_path = std::nullopt;
            return;
        }

        _texture_slots.clear();
        _texture_names.clear();
        _material_path = std::move(name);
        _needs_update_material = true;
    }

    void mesh3d_node::bind_node_texture_slot(const std::string& texture_name, uint32_t slot)
    {
        if (slot >= _texture_names.size())
        {
            _texture_names.resize(slot + 1, DEFAULT_TEXTURE_NAME);
        }
        _texture_names[slot] = texture_name;
        _needs_update_textures = true;
    }

    void mesh3d_node::bind_node_texture_slot(const renderer& rend, std::shared_ptr<texture> tex, uint32_t slot)
    {
        vk::DescriptorImageInfo info;
        info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        info.imageView = tex->imageview();
        info.sampler = tex->sampler().get_sampler();

        vk::WriteDescriptorSet write;
        write.pImageInfo = &info;
        write.descriptorCount = 1;
        write.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        write.dstArrayElement = slot;
        write.dstBinding = 1;
        write.dstSet = *_descriptor_set;
        write.pTexelBufferView = nullptr;

        rend.vkctx().device().updateDescriptorSets(write, {});

        if (slot >= _texture_slots.size())
        {
            _texture_slots.resize(slot + 1);
        }
        _texture_slots[slot] = std::move(tex);
    }

    void mesh3d_node::tick(scene& scene, double deltatime)
    {
        node::tick(scene, deltatime);

        if (_needs_update_material)
        {
            update_material(scene);
        }

        if (_material == nullptr)
        {
            return;
        }

        if (!_mesh && _mesh_path)
        {
            _mesh = scene.load_mesh(*_mesh_path);
        }

        if (_needs_refresh_buffers)
        {
            if (_mesh_path.has_value())
            {
                _mesh_buffers = scene.get_mesh_buffers(*_mesh_path, *_mesh);
                _needs_refresh_buffers = false;
            }
            else
            {
                return;
            }
        }

        if (_needs_update_textures)
        {
            for (uint32_t i = 0; i < _texture_names.size(); ++i)
            {
                const auto& tex_name = _texture_names[i];
                if(tex_name == DEFAULT_TEXTURE_NAME)
                {
                    continue;
                }
                bind_node_texture_slot(scene.get_renderer(), scene.load_texture(tex_name), i);
            }
            _needs_update_textures = false;
        }

        const auto& definition = _material->definition();
        const auto& node_bindings = definition.node_uniform_bindings();

        if (node_bindings.contains(shader_uniform_binding::NODE_MODEL_MATRIX))
        {
            const auto offset = node_bindings.at(shader_uniform_binding::NODE_MODEL_MATRIX);
            const auto& model = get_world_model_matrix();
            CRITICAL_CHECK(_uniform_data.size() >= offset + sizeof(model), "Attempt to write beyond bounds of uniform data");
            *reinterpret_cast<glm::mat4*>(_uniform_data.data() + offset) = model;
            _uniform_needs_update = true;
        }

        if (node_bindings.contains(shader_uniform_binding::NODE_ID))
        {
            const auto offset = node_bindings.at(shader_uniform_binding::NODE_ID);
            CRITICAL_CHECK(_uniform_data.size() >= offset + sizeof(_id), "Attempt to write beyond bounds of uniform data");
            *reinterpret_cast<std::remove_const_t<decltype(_id)>*>(_uniform_data.data() + offset) = _id;
            _uniform_needs_update = true;
        }

        if (_uniform_needs_update)
        {
            scene.get_renderer().get_upload_queue().update_buffer(*_mesh3d_uniform_buffer, 0, _uniform_data);
            _uniform_needs_update = false;
        }

        auto& [vxbuff, ixbuff] = *_mesh_buffers;

        const auto cmdbuff_type = [&] {
            switch (_material->definition().domain())
            {
            case material_definition_domain::OPAQUE:
                return render_cmdbuff_type::OPAQUE;
            case material_definition_domain::TRANSPARENT:
                return render_cmdbuff_type::TRANSPARENT;
            case material_definition_domain::OVERLAY:
                return render_cmdbuff_type::OVERLAY;
            default:
                CRITICAL_ERROR("Unhandled material definition domain");
            }
        }();

        vk::CommandBuffer cmdbuff = scene.get_renderer().render_cmdbuff(cmdbuff_type);
        cmdbuff.bindPipeline(vk::PipelineBindPoint::eGraphics, _material->pipeline().get());
        cmdbuff.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            _material->pipeline().pipeline_layout(),
            0,
            { scene.descriptor_set(), _material->descriptor_set(), *_descriptor_set },
            {});
        cmdbuff.bindVertexBuffers(0, vxbuff.buffer(), { 0 });
        cmdbuff.bindIndexBuffer(ixbuff.buffer(), 0, vk::IndexType::eUint32);
        cmdbuff.drawIndexed(ixbuff.index_count(), 1, 0, 0, 0);
    }

    void mesh3d_node::init_default_textures(const renderer& rend)
    {
        const auto defs = _material->node_descriptor_set_definition();
        if (defs.definition.entries.size() > 1)
        {
            for (uint32_t i = 0; i < defs.definition.entries[1].count; ++i)
            {
                bind_node_texture_slot(rend, rend.default_texture(), i);
            }
            _texture_names.resize(defs.definition.entries[1].count, DEFAULT_TEXTURE_NAME);
        }
    }

    void mesh3d_node::update_material(scene& scn)
    {
        if (!_material_path.has_value())
        {
            return;
        }

        if (scn.get_renderer().materials().contains(*_material_path))
        {
            _material = scn.get_renderer().materials().at(*_material_path);
        }
        else
        {
            _material = scn.load_material(*_material_path);
        }

        if (_material != nullptr)
        {
            auto& renderer = _material->get_renderer();
            const auto node_uniform_size = _material->definition().node_uniform_block_size();
            if ((node_uniform_size != 0U) && _uniform_data.size() != node_uniform_size)
            {
                _uniform_data.resize(node_uniform_size);
                _mesh3d_uniform_buffer.reset();

                if (node_uniform_size > 0)
                {
                    gfx::uniform_buffer_args buff_args;
                    buff_args.size = _material->definition().node_uniform_block_size();
                    buff_args.vkctx = &renderer.vkctx();

                    _mesh3d_uniform_buffer = std::make_unique<gfx::uniform_buffer>(buff_args);
                }
            }

            if (!_descriptor_set)
            {
                const auto layout = _material->node_descriptor_set_layout();
                vk::DescriptorSetAllocateInfo alloc_info;
                alloc_info.descriptorPool = renderer.vkctx().descriptor_pool();
                alloc_info.descriptorSetCount = 1;
                alloc_info.pSetLayouts = &layout;

                _descriptor_set = std::move(renderer.vkctx().device().allocateDescriptorSetsUnique(alloc_info)[0]);
            }

            const auto& buffer = _mesh3d_uniform_buffer ? _mesh3d_uniform_buffer : renderer.empty_uniform_buffer();

            vk::DescriptorBufferInfo buffer_info;
            buffer_info.buffer = buffer->buffer();
            buffer_info.offset = 0;
            buffer_info.range = buffer->size();

            vk::WriteDescriptorSet write;
            write.descriptorCount = 1;
            write.descriptorType = vk::DescriptorType::eUniformBuffer;
            write.pBufferInfo = &buffer_info;
            write.dstArrayElement = 0;
            write.dstBinding = 0;
            write.dstSet = *_descriptor_set;
            renderer.vkctx().device().updateDescriptorSets(write, {});

            init_default_textures(renderer);
        }
        _needs_update_material = false;
    }
} // namespace cathedral::engine