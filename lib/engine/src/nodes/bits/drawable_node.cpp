#include <cathedral/engine/nodes/bits/drawable_node.hpp>

#include <cathedral/engine/scene.hpp>

namespace cathedral::engine
{
    void drawable_node::set_mesh(std::optional<std::string> name)
    {
        if ((_mesh_name.has_value() != name.has_value()) || (name.has_value() && (_mesh_name.value() != name.value())))
        {
            _mesh_name = std::move(name);
            _needs_update_mesh = true;
        }
    }

    void drawable_node::set_mesh(std::shared_ptr<mesh_buffer> mesh_buffer)
    {
        _mesh_buffers = std::move(mesh_buffer);
        _mesh_name = std::nullopt;
        _needs_update_mesh = false;
    }

    void drawable_node::set_material(std::optional<std::string> name)
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

    void drawable_node::bind_node_texture_slot(const std::string& texture_name, uint32_t slot)
    {
        if (slot >= _texture_names.size())
        {
            _texture_names.resize(slot + 1, DEFAULT_TEXTURE_NAME);
        }
        _texture_names[slot] = texture_name;
        _needs_update_textures = true;
    }

    void drawable_node::bind_node_texture_slot(const renderer& rend, std::shared_ptr<texture> tex, const uint32_t slot)
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
        _texture_slots[slot] = tex;

        if (slot >= _texture_names.size())
        {
            _texture_names.resize(slot + 1, DEFAULT_TEXTURE_NAME);
        }
        _texture_names[slot] = tex->name();
    }

    void drawable_node::tick_setup(scene& scene)
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

        for (uint32_t i = 0; i < _needs_update_buffers.size(); ++i)
        {
            if (_needs_update_buffers[i])
            {
                update_storage_buffer(scene, STORAGE_BUFFER_FIRST_BINDING_INDEX + i);
                _needs_update_buffers[i] = false;
            }
        }
    }

    void drawable_node::tick(scene& scene, const double deltatime)
    {
        node::tick(scene, deltatime);
        render(scene);
    }

    void drawable_node::editor_tick(scene& scene, double deltatime)
    {
        node::editor_tick(scene, deltatime);
        render(scene);
    }

    void drawable_node::update_uniform(const std::function<void(std::span<std::byte>&)>& func)
    {
        if (_uniform_data.empty())
        {
            return;
        }

        const auto previous_data = _uniform_data;
        auto span = std::span{ _uniform_data.data(), _uniform_data.size() };
        func(span);
        if (previous_data != _uniform_data)
        {
            _uniform_needs_update = true;
        }
    }

    void drawable_node::force_refresh_uniform()
    {
        _uniform_needs_update = true;
    }

    void drawable_node::set_storage_buffer_data(const uint32_t binding_index, std::vector<std::byte> data)
    {
        CRITICAL_CHECK(binding_index < _node_storage_buffers.size(), "Node storage buffer binding index out of range");

        _node_storage_buffers_data[binding_index] = std::move(data);
        _needs_update_buffers[binding_index] = true;
    }

    void drawable_node::init_default_textures(const renderer& rend)
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

    void drawable_node::update_storage_buffer(const scene& scene, const uint32_t binding_index)
    {
        CRITICAL_CHECK(binding_index >= STORAGE_BUFFER_FIRST_BINDING_INDEX, "Invalid storage buffer binding index");

        const uint32_t buffer_index = binding_index - STORAGE_BUFFER_FIRST_BINDING_INDEX;

        auto& renderer = scene.get_renderer();
        const auto& data = _node_storage_buffers_data[buffer_index];

        if (data.empty())
        {
            _node_storage_buffers[buffer_index] = scene.get_renderer().default_storage_buffer();
        }
        else if (_node_storage_buffers[buffer_index]->size() != data.size())
        {
            gfx::storage_buffer_args args;
            args.size = data.size();
            args.vkctx = &renderer.vkctx();

            _node_storage_buffers[buffer_index] = std::make_shared<gfx::storage_buffer>(std::move(args));
        }

        renderer.get_upload_queue().update_buffer(*_node_storage_buffers[buffer_index], 0, data);

        vk::DescriptorBufferInfo buffer_info;
        buffer_info.buffer = _node_storage_buffers[buffer_index]->buffer();
        buffer_info.offset = 0;
        buffer_info.range = _node_storage_buffers[buffer_index]->size();

        vk::WriteDescriptorSet write;
        write.descriptorCount = 1;
        write.dstBinding = binding_index;
        write.dstSet = *_descriptor_set;
        write.pBufferInfo = &buffer_info;
        write.descriptorType = vk::DescriptorType::eStorageBuffer;
        write.dstArrayElement = 0;
        write.pImageInfo = nullptr;
        write.pTexelBufferView = nullptr;

        renderer.vkctx().device().updateDescriptorSets(write, {});
    }

    void drawable_node::update_material(scene& scene)
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

            const auto initialize_uniform_buffer = [this, &material, &renderer] {
                gfx::uniform_buffer_args buff_args;
                buff_args.size = material->node_uniform_block_size();
                buff_args.vkctx = &renderer.vkctx();

                _node_uniform_buffer = std::make_unique<gfx::uniform_buffer>(buff_args);
            };

            // If the node uniform block size has changed, resize the uniform data block
            // and regenerate the corresponding uniform buffer
            if (const auto node_uniform_size = material->node_uniform_block_size();
                (node_uniform_size != 0U) && _uniform_data.size() != node_uniform_size)
            {
                _uniform_data.resize(node_uniform_size);
                _uniform_needs_update = true;
                _node_uniform_buffer.reset();

                if (node_uniform_size > 0)
                {
                    initialize_uniform_buffer();
                }
            }
            else if (_node_uniform_buffer == nullptr) // Special case for when the uniform data is initialized before the
                                                      // buffer (i.e. deserialization)
            {
                initialize_uniform_buffer();
            }

            const auto layout = material->node_descriptor_set_layout();
            vk::DescriptorSetAllocateInfo alloc_info;
            alloc_info.descriptorPool = renderer.vkctx().descriptor_pool();
            alloc_info.descriptorSetCount = 1;
            alloc_info.pSetLayouts = &layout;
            _descriptor_set = std::move(renderer.vkctx().device().allocateDescriptorSetsUnique(alloc_info)[0]);

            const auto& buffer = _node_uniform_buffer ? _node_uniform_buffer : renderer.empty_uniform_buffer();

            vk::DescriptorBufferInfo buffer_info;
            buffer_info.buffer = buffer->buffer();
            buffer_info.offset = 0;
            buffer_info.range = buffer->size();

            vk::WriteDescriptorSet write_uniform;
            write_uniform.descriptorCount = 1;
            write_uniform.descriptorType = vk::DescriptorType::eUniformBuffer;
            write_uniform.pBufferInfo = &buffer_info;
            write_uniform.dstArrayElement = 0;
            write_uniform.dstBinding = 0;
            write_uniform.dstSet = *_descriptor_set;

            // Initialize storage buffers with empty defaults
            _node_storage_buffers.clear();
            _node_storage_buffers_data.clear();
            _needs_update_buffers.clear();

            for (uint32_t i = 0; i < _material.lock()->node_buffer_names().size(); ++i)
            {
                _node_storage_buffers.push_back(_material.lock()->get_renderer().default_storage_buffer());

                std::vector buffer_data(4, static_cast<std::byte>(0));
                _node_storage_buffers_data.push_back(std::move(buffer_data));

                _needs_update_buffers.push_back(true);
            }

            renderer.vkctx().device().updateDescriptorSets(write_uniform, {});

            init_default_textures(renderer);
        }
        _needs_update_material = false;
    }

    void drawable_node::update_textures(scene& scene)
    {
        for (uint32_t i = 0; i < _texture_names.size(); ++i)
        {
            const auto& tex_name = _texture_names[i];
            if (tex_name == DEFAULT_TEXTURE_NAME)
            {
                continue;
            }

            auto texture = scene.load_texture(tex_name);
            if (texture == nullptr)
            {
                log_error(
                    std::format(
                        "Node '{}' has node texture '{}' in slot '{}', which cannot found in project assets. "
                        "It will be replaced with the default texture.",
                        get_full_name(" > "),
                        tex_name,
                        i));
                _texture_names[i] = DEFAULT_TEXTURE_NAME;
                auto tex = scene.get_renderer().default_texture();
                bind_node_texture_slot(scene.get_renderer(), std::move(tex), i);
            }
            else
            {
                bind_node_texture_slot(scene.get_renderer(), std::move(texture), i);
            }
        }
        _needs_update_textures = false;
    }

    void drawable_node::update_bindings()
    {
        if (_material.expired())
        {
            return;
        }

        const auto material = _material.lock();

        for (const auto& [var_name, binding] : material->node_uniform_bindings())
        {
            if (binding == shader_node_uniform_binding::NODE_MODEL_MATRIX)
            {
                const auto offset = *material->get_node_uniform_var_offset(var_name);

                const auto& model = world_model_matrix();
                CRITICAL_CHECK(
                    _uniform_data.size() >= offset + sizeof(model),
                    "Attempt to write beyond bounds of uniform data");
                if (auto* ptr = reinterpret_cast<glm::mat4*>(_uniform_data.data() + offset); *ptr != model)
                {
                    *ptr = model;
                    _uniform_needs_update = true;
                }
            }
            else if (binding == shader_node_uniform_binding::NODE_ID)
            {
                const auto offset = *material->get_node_uniform_var_offset(var_name);

                CRITICAL_CHECK(
                    _uniform_data.size() >= offset + sizeof(_uid),
                    "Attempt to write beyond bounds of uniform data");
                if (auto* ptr = reinterpret_cast<std::remove_const_t<decltype(_uid)>*>(_uniform_data.data() + offset);
                    *ptr != _uid)
                {
                    *ptr = _uid;
                    _uniform_needs_update = true;
                }
            }
        }
    }
} // namespace cathedral::engine