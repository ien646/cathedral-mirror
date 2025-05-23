#include <cathedral/engine/nodes/mesh3d_node.hpp>

#include <cathedral/engine/scene.hpp>

namespace cathedral::engine
{
    void mesh3d_node::set_mesh(std::optional<std::string> name)
    {
        if ((_mesh_name.has_value() != name.has_value()) || (name.has_value() && (_mesh_name.value() != name.value())))
        {
            _mesh_name = std::move(name);
            _mesh_buffers = {};
            _needs_update_mesh = true;
        }
    }

    void mesh3d_node::set_mesh(std::shared_ptr<mesh_buffer> mesh_buffer)
    {
        _mesh_buffers = std::move(mesh_buffer);
        _mesh_name = std::nullopt;
        _needs_update_mesh = false;
    }

    void mesh3d_node::set_material(std::optional<std::string> name)
    {
        if (!name.has_value())
        {
            _material_name = std::nullopt;
            return;
        }

        _texture_slots.clear();
        _texture_names.clear();
        _material_name = std::move(name);
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

    void mesh3d_node::bind_node_texture_slot(const renderer& rend, std::shared_ptr<texture> tex, const uint32_t slot)
    {
        if (_material.expired())
        {
            return;
        }

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

    void mesh3d_node::tick_setup(scene& scene)
    {
        node::tick_setup(scene);

        if (_needs_update_material || (!_material.expired() && (_material.lock()->uid() != _material_uid)))
        {
            update_material(scene);
        }

        if (_needs_update_textures)
        {
            update_textures(scene);
        }
    }

    void mesh3d_node::tick(scene& scene, const double deltatime)
    {
        node::tick(scene, deltatime);

        if (_disabled || (_disabled_in_editor && scene.in_editor_mode()))
        {
            return;
        }

        if (_material.expired())
        {
            if (_material_name.has_value())
            {
                _material = scene.load_material(*_material_name);
                _material_uid = _material.expired() ? std::numeric_limits<uint32_t>::max() : _material.lock()->uid();
                _needs_update_material = true;
            }
            else
            {
                _material = {};
                _material_uid = std::numeric_limits<uint32_t>::max();
                _needs_update_material = true;
            }
            // Avoid rendering the current frame, since modified material resources have to be
            // recreated (i.e. descriptors), and doing it mid frame is a no-no
            return;
        }

        if (_needs_update_mesh)
        {
            _needs_update_mesh = false;
            if (_mesh_name.has_value())
            {
                _mesh = scene.load_mesh(*_mesh_name);
                _mesh_buffers = scene.get_mesh_buffers(*_mesh_name, *_mesh);
            }
            else
            {
                return;
            }
        }

        if (_mesh_buffers == nullptr)
        {
            return;
        }

        const auto material = _material.lock();

        update_bindings();

        if (_uniform_needs_update)
        {
            scene.get_renderer().get_upload_queue().update_buffer(*_mesh3d_uniform_buffer, 0, _uniform_data);
            _uniform_needs_update = false;
        }

        auto& [vxbuff, ixbuff] = *_mesh_buffers;

        const auto cmdbuff_type = [&] {
            switch (material->domain())
            {
            case material_domain::OPAQUE:
                return render_cmdbuff_type::OPAQUE;
            case material_domain::TRANSPARENT:
                return render_cmdbuff_type::TRANSPARENT;
            case material_domain::OVERLAY:
                return render_cmdbuff_type::OVERLAY;
            default:
                CRITICAL_ERROR("Unhandled material definition domain");
            }
        }();

        const vk::CommandBuffer cmdbuff = scene.get_renderer().render_cmdbuff(cmdbuff_type);
        cmdbuff.bindPipeline(vk::PipelineBindPoint::eGraphics, material->pipeline().get());
        cmdbuff.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            material->pipeline().pipeline_layout(),
            0,
            { scene.descriptor_set(), material->descriptor_set(), *_descriptor_set },
            {});
        cmdbuff.bindVertexBuffers(0, vxbuff.buffer(), { 0 });
        cmdbuff.bindIndexBuffer(ixbuff.buffer(), 0, vk::IndexType::eUint32);
        cmdbuff.drawIndexed(ixbuff.index_count(), 1, 0, 0, 0);
    }

    std::shared_ptr<scene_node> mesh3d_node::copy(const std::string& name, bool copy_children) const
    {
        auto result = std::make_shared<mesh3d_node>(name, _parent, !_disabled);

        result->set_local_transform(_local_transform);

        if (_mesh_name.has_value())
        {
            result->set_mesh(_mesh_name);
        }
        result->set_material(_material_name);

        for (uint32_t i = 0; i < static_cast<uint32_t>(_texture_slots.size()); ++i)
        {
            result->bind_node_texture_slot(_texture_names[i], i);
        }

        if (copy_children)
        {
            copy_children_into(*result);
        }

        return result;
    }

    void mesh3d_node::init_default_textures(const renderer& rend)
    {
        const auto defs = _material.lock()->node_descriptor_set_definition();
        if (defs.definition.entries.size() > 1)
        {
            for (uint32_t i = 0; i < defs.definition.entries[1].count; ++i)
            {
                if (i < _texture_names.size())
                {
                    bind_node_texture_slot(_texture_names[i], i);
                }
                else
                {
                    bind_node_texture_slot(rend, rend.default_texture(), i);
                }
            }
            _texture_names.resize(defs.definition.entries[1].count, DEFAULT_TEXTURE_NAME);
        }
    }

    void mesh3d_node::update_material(scene& scene)
    {
        if (!_material_name.has_value())
        {
            _needs_update_material = false;
            return;
        }

        if (scene.get_renderer().materials().contains(*_material_name))
        {
            _material = scene.get_renderer().materials().at(*_material_name);
            _material_uid = _material.expired() ? std::numeric_limits<uint32_t>::max() : _material.lock()->uid();
        }
        else
        {
            _material = scene.load_material(*_material_name);
            _material_uid = _material.expired() ? std::numeric_limits<uint32_t>::max() : _material.lock()->uid();
        }

        if (!_material.expired())
        {
            const auto& material = _material.lock();
            const auto& renderer = material->get_renderer();

            if (const auto node_uniform_size = material->node_uniform_block_size();
                (node_uniform_size != 0U) && _uniform_data.size() != node_uniform_size)
            {
                _uniform_data.resize(node_uniform_size);
                _mesh3d_uniform_buffer.reset();

                if (node_uniform_size > 0)
                {
                    gfx::uniform_buffer_args buff_args;
                    buff_args.size = material->node_uniform_block_size();
                    buff_args.vkctx = &renderer.vkctx();

                    _mesh3d_uniform_buffer = std::make_unique<gfx::uniform_buffer>(buff_args);
                }
            }

            const auto layout = material->node_descriptor_set_layout();
            vk::DescriptorSetAllocateInfo alloc_info;
            alloc_info.descriptorPool = renderer.vkctx().descriptor_pool();
            alloc_info.descriptorSetCount = 1;
            alloc_info.pSetLayouts = &layout;
            _descriptor_set = std::move(renderer.vkctx().device().allocateDescriptorSetsUnique(alloc_info)[0]);

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

    void mesh3d_node::update_textures(scene& scene)
    {
        for (uint32_t i = 0; i < _texture_names.size(); ++i)
        {
            const auto& tex_name = _texture_names[i];
            if (tex_name == DEFAULT_TEXTURE_NAME)
            {
                continue;
            }

            if (auto texture = scene.load_texture(tex_name); texture != nullptr)
            {
                bind_node_texture_slot(scene.get_renderer(), std::move(texture), i);
            }
        }
        _needs_update_textures = false;
    }

    void mesh3d_node::update_bindings()
    {
        if (_material.expired())
        {
            return;
        }

        const auto material = _material.lock();

        if (material->node_bindings().contains(shader_node_uniform_binding::NODE_MODEL_MATRIX))
        {
            const auto& var_name = material->node_bindings().at(shader_node_uniform_binding::NODE_MODEL_MATRIX);
            const auto offset = material->get_node_binding_var_offset(var_name);

            const auto& model = get_world_model_matrix();
            CRITICAL_CHECK(_uniform_data.size() >= offset + sizeof(model), "Attempt to write beyond bounds of uniform data");
            if (auto* ptr = reinterpret_cast<glm::mat4*>(_uniform_data.data() + offset); *ptr != model)
            {
                *ptr = model;
                _uniform_needs_update = true;
            }
        }

        if (material->node_bindings().contains(shader_node_uniform_binding::NODE_ID))
        {
            const auto& var_name = material->node_bindings().at(shader_node_uniform_binding::NODE_ID);
            const auto offset = material->get_node_binding_var_offset(var_name);

            CRITICAL_CHECK(_uniform_data.size() >= offset + sizeof(_uid), "Attempt to write beyond bounds of uniform data");
            if (auto* ptr = reinterpret_cast<std::remove_const_t<decltype(_uid)>*>(_uniform_data.data() + offset);
                *ptr != _uid)
            {
                *ptr = _uid;
                _uniform_needs_update = true;
            }
        }
    }
} // namespace cathedral::engine