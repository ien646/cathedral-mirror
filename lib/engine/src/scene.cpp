#include <cathedral/engine/scene.hpp>

namespace cathedral::engine
{
    scene::scene(renderer& renderer)
        : _renderer(renderer)
        , _mesh_buffer_storage(renderer)
    {
        gfx::uniform_buffer_args uniform_buffer_args;
        uniform_buffer_args.size = sizeof(scene_uniform_data);
        uniform_buffer_args.vkctx = &_renderer.vkctx();

        _uniform_buffer = std::make_unique<gfx::uniform_buffer>(uniform_buffer_args);

        init_descriptor_set_layout();
        init_descriptor_set();

        _previous_frame_timepoint = scene_clock::now();
    }

    vk::DescriptorSet scene::descriptor_set() const
    {
        return *_descriptor_set;
    }

    void scene::tick(std::function<void(double deltatime)> func)
    {
        const auto now = scene_clock::now();
        const auto deltatime_ns =
            std::chrono::duration_cast<std::chrono::nanoseconds>(now - _previous_frame_timepoint).count();
        const double deltatime_s = static_cast<double>(deltatime_ns) / 1'000'000'000;
        _previous_frame_timepoint = now;

        _renderer.begin_frame();

        func(deltatime_s);

        _uniform_data.deltatime = deltatime_s;
        _uniform_data.frame_index = _renderer.current_frame();
        _renderer.get_upload_queue().update_buffer(*_uniform_buffer, 0, &_uniform_data, sizeof(_uniform_data));

        for (auto& [name, mat] : _renderer.materials())
        {
            mat->update();
        }

        for (auto& [name, node] : _root_nodes)
        {
            node->tick(deltatime_s);
        }

        _renderer.end_frame();
    }

    std::shared_ptr<scene_node> scene::get_node(const std::string& name)
    {
        if (_root_nodes.count(name))
        {
            return _root_nodes.at(name);
        }
        return nullptr;
    }

    void scene::update_uniform(std::function<void(scene_uniform_data&)> func)
    {
        func(_uniform_data);
    }

    std::shared_ptr<mesh_buffer> scene::get_mesh_buffers(const std::string& mesh_path)
    {
        return _mesh_buffer_storage.get_mesh_buffers(mesh_path);
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

    void scene::init_descriptor_set_layout()
    {
        const auto dset_definition = descriptor_set_definition();
        _descriptor_set_layout = dset_definition.definition.create_descriptor_set_layout(_renderer.vkctx());
    }

    void scene::init_descriptor_set()
    {
        vk::DescriptorSetAllocateInfo alloc_info;
        alloc_info.descriptorPool = _renderer.vkctx().descriptor_pool();
        alloc_info.descriptorSetCount = 1;
        alloc_info.pSetLayouts = &*_descriptor_set_layout;

        _descriptor_set = std::move(_renderer.vkctx().device().allocateDescriptorSetsUnique(alloc_info)[0]);

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
        write.dstSet = *_descriptor_set;
        _renderer.vkctx().device().updateDescriptorSets(write, {});
    }
} // namespace cathedral::engine