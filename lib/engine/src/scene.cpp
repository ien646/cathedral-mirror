#include <cathedral/engine/scene.hpp>

namespace cathedral::engine
{
    scene::scene(scene_args args)
        : _args(std::move(args))
        , _mesh_buffer_storage(*_args.prenderer)
    {
        CRITICAL_CHECK(_args.loaders.mesh_loader != nullptr);
        CRITICAL_CHECK(_args.loaders.texture_loader != nullptr);

        gfx::uniform_buffer_args uniform_buffer_args;
        uniform_buffer_args.size = sizeof(scene_uniform_data);
        uniform_buffer_args.vkctx = &get_renderer().vkctx();

        _uniform_buffer = std::make_unique<gfx::uniform_buffer>(uniform_buffer_args);

        init_descriptor_set_layout();
        init_descriptor_set();

        _previous_frame_timepoint = scene_clock::now();
    }

    vk::DescriptorSet scene::descriptor_set() const
    {
        return *_scene_descriptor_set;
    }

    void scene::tick(const std::function<void(double deltatime)>& func)
    {
        const auto now = scene_clock::now();
        const auto deltatime_ns =
            std::chrono::duration_cast<std::chrono::nanoseconds>(now - _previous_frame_timepoint).count();
        const double deltatime_s = static_cast<double>(deltatime_ns) / 1'000'000'000;
        _previous_frame_timepoint = now;

        get_renderer().begin_frame();

        func(deltatime_s);

        _scene_uniform_data.deltatime = static_cast<float>(deltatime_s);
        _scene_uniform_data.frame_index = static_cast<uint32_t>(get_renderer().current_frame());
        get_renderer().get_upload_queue().update_buffer(
            *_uniform_buffer,
            0,
            std::span<const scene_uniform_data>{ &_scene_uniform_data, 1 });

        for (const auto& [name, mat] : get_renderer().materials())
        {
            mat->update();
        }

        for (const auto& [name, node] : _root_nodes)
        {
            node->tick(*this, deltatime_s);
        }

        get_renderer().end_frame();
    }

    std::shared_ptr<scene_node> scene::get_node(const std::string& name)
    {
        if (_root_nodes.contains(name))
        {
            return _root_nodes.at(name);
        }
        return nullptr;
    }

    void scene::update_uniform(const std::function<void(scene_uniform_data&)>& func)
    {
        func(_scene_uniform_data);
    }

    std::shared_ptr<mesh_buffer> scene::get_mesh_buffers(const std::string& mesh_path, const engine::mesh& mesh)
    {
        return _mesh_buffer_storage.get_mesh_buffers(mesh_path, mesh);
    }

    gfx::pipeline_descriptor_set scene::descriptor_set_definition()
    {
        gfx::pipeline_descriptor_set result;
        result.set_index = 0;
        result.definition.entries = {
            gfx::descriptor_set_entry(result.set_index, 0, gfx::descriptor_type::UNIFORM, 1) // scene uniform data
        };

        return result;
    }

    std::shared_ptr<engine::mesh> scene::load_mesh(const std::string& absolute_path) const
    {
        return _args.loaders.mesh_loader(absolute_path, *this);
    }

    std::shared_ptr<engine::texture> scene::load_texture(const std::string& absolute_path) const
    {
        return _args.loaders.texture_loader(absolute_path, *this);
    }

    void scene::init_descriptor_set_layout()
    {
        const auto dset_definition = descriptor_set_definition();
        _scene_descriptor_set_layout = dset_definition.definition.create_descriptor_set_layout(get_renderer().vkctx());
    }

    void scene::init_descriptor_set()
    {
        vk::DescriptorSetAllocateInfo alloc_info;
        alloc_info.descriptorPool = get_renderer().vkctx().descriptor_pool();
        alloc_info.descriptorSetCount = 1;
        alloc_info.pSetLayouts = &*_scene_descriptor_set_layout;

        _scene_descriptor_set = std::move(get_renderer().vkctx().device().allocateDescriptorSetsUnique(alloc_info)[0]);

        vk::DescriptorBufferInfo buffer_info;
        buffer_info.buffer = _uniform_buffer->buffer();
        buffer_info.offset = 0;
        buffer_info.range = _uniform_buffer->size();

        vk::WriteDescriptorSet write;
        write.descriptorCount = 1;
        write.descriptorType = vk::DescriptorType::eUniformBuffer;
        write.pBufferInfo = &buffer_info;
        write.dstArrayElement = 0;
        write.dstBinding = 0;
        write.dstSet = *_scene_descriptor_set;
        get_renderer().vkctx().device().updateDescriptorSets(write, {});
    }
} // namespace cathedral::engine