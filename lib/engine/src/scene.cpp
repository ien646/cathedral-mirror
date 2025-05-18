#include "cathedral/engine/nodes/point_light_node.hpp"

#include <cathedral/engine/scene.hpp>

#include <cathedral/engine/nodes/camera2d_node.hpp>
#include <cathedral/engine/nodes/camera3d_node.hpp>
#include <cathedral/engine/nodes/mesh3d_node.hpp>
#include <cathedral/engine/nodes/node.hpp>

#include <ien/algorithm.hpp>

#include <print>
#include <ranges>

namespace cathedral::engine
{
    namespace
    {
        void reload_node_parenting(const std::shared_ptr<scene_node>& node, scene_node* parent)
        {
            if (parent != nullptr)
            {
                node->set_parent(parent);
            }

            for (const auto& child : node->children())
            {
                reload_node_parenting(child, node.get());
            }
        }
    } // namespace

    scene::scene(scene_args args)
        : _args(std::move(args))
        , _mesh_buffer_storage(_args.prenderer)
    {
        CRITICAL_CHECK_NOTNULL(_args.loaders.material_loader);
        CRITICAL_CHECK_NOTNULL(_args.loaders.mesh_loader);
        CRITICAL_CHECK_NOTNULL(_args.loaders.texture_loader);

        gfx::uniform_buffer_args uniform_buffer_args;
        uniform_buffer_args.size = sizeof(scene_uniform_data);
        uniform_buffer_args.vkctx = &get_renderer().vkctx();

        _uniform_buffer = std::make_unique<gfx::uniform_buffer>(uniform_buffer_args);

        init_descriptor_set_layout();
        init_descriptor_set();

        _previous_frame_timepoint = scene_clock::now();
    }

    scene::~scene()
    {
        // Wait for any in-flight commands before deleting scene related resources
        try
        {
            _args.prenderer->vkctx().device().waitIdle();
        }
        catch (const std::exception&)
        {
            std::print("Failure waiting for vulkan device");
            std::exit(-1);
        }
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

        _used_point_lights = 0;

        get_renderer().begin_frame();

        func(deltatime_s);
        _last_deltatime = deltatime_s;

        _scene_uniform_data.deltatime = static_cast<float>(deltatime_s);
        _scene_uniform_data.frame_index = static_cast<uint32_t>(get_renderer().current_frame());
        get_renderer().get_upload_queue().update_buffer(
            *_uniform_buffer,
            0,
            std::span<const scene_uniform_data>{ &_scene_uniform_data, 1 });

        for (const auto& mat : get_renderer().materials() | std::views::values)
        {
            mat->update();
        }

        for (const auto& node : _root_nodes)
        {
            if (node->enabled())
            {
                node->tick_setup(*this);
            }
        }

        if (_in_editor)
        {
            for (const auto& node : _root_nodes)
            {
                if (node->enabled())
                {
                    node->editor_tick(*this, deltatime_s);
                }
            }
        }
        else
        {
            for (const auto& node : _root_nodes)
            {
                if (node->enabled())
                {
                    node->tick(*this, deltatime_s);
                }
            }
        }

        _scene_uniform_data.enabled_point_lights = _used_point_lights;

        get_renderer().end_frame();
    }

    std::shared_ptr<scene_node> scene::add_root_node(const std::string& name, const node_type type)
    {
        switch (type)
        {
        case node_type::NODE:
            return add_root_node<node>(name);
        case node_type::MESH3D_NODE:
            return add_root_node<mesh3d_node>(name);
        case node_type::CAMERA2D_NODE:
            return add_root_node<camera2d_node>(name);
        case node_type::CAMERA3D_NODE:
            return add_root_node<camera3d_node>(name);
        case node_type::POINT_LIGHT:
            return add_root_node<point_light_node>(name);
        default:
            CRITICAL_ERROR("Unhandled node type");
        }
    }

    void scene::add_root_node(std::shared_ptr<scene_node> node)
    {
        _root_nodes.push_back(std::move(node));
    }

    std::shared_ptr<scene_node> scene::get_node(const std::string& name)
    {
        const auto it = std::ranges::find_if(_root_nodes, [&name](const std::shared_ptr<scene_node>& node) {
            return node->name() == name;
        });

        if (it != _root_nodes.end())
        {
            return *it;
        }
        return nullptr;
    }

    void scene::remove_node(const std::string& name)
    {
        const auto it = std::ranges::find_if(_root_nodes, [&name](const std::shared_ptr<engine::scene_node>& node) {
            return node->name() == name;
        });

        CRITICAL_CHECK(it != _root_nodes.end(), "Node not found");
        ien::erase_unsorted(_root_nodes, it);
    }

    bool scene::contains_node(const std::string& name) const
    {
        return std::ranges::find_if(_root_nodes, [&name](const std::shared_ptr<engine::scene_node>& node) {
                   return node->name() == name;
               }) != _root_nodes.end();
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

    std::weak_ptr<material> scene::load_material(const std::string& name)
    {
        return _args.loaders.material_loader(name, *this);
    }

    std::shared_ptr<mesh> scene::load_mesh(const std::string& name)
    {
        return _args.loaders.mesh_loader(name, *this);
    }

    std::shared_ptr<texture> scene::load_texture(const std::string& name)
    {
        return _args.loaders.texture_loader(name, *this);
    }

    void scene::load_nodes(std::vector<std::shared_ptr<scene_node>>&& nodes)
    {
        _root_nodes = std::move(nodes);
        reload_tree_parenting();
    }

    void scene::set_frame_point_light(const point_light_data& data)
    {
        if (_used_point_lights < MAX_SCENE_POINT_LIGHTS)
        {
            _scene_uniform_data.point_lights[_used_point_lights++] = data;
        }
    }

    void scene::set_in_editor_mode(const bool in_editor)
    {
        _in_editor = in_editor;
    }

    bool scene::in_editor_mode() const
    {
        return _in_editor;
    }

    namespace
    {
        void get_nodes_of_type(const node_type type, const std::shared_ptr<scene_node>& node, std::vector<std::shared_ptr<scene_node>>& target)
        {
            if (node->type() == type)
            {
                target.push_back(node);
            }
            for (const std::shared_ptr<scene_node>& child : node->children())
            {
                get_nodes_of_type(type, child, target);
            }
        }
    }

    std::vector<std::shared_ptr<scene_node>> scene::get_nodes_by_type(const node_type type) const
    {
        std::vector<std::shared_ptr<scene_node>> result;
        for (const std::shared_ptr<scene_node>& node : _root_nodes)
        {
            get_nodes_of_type(type, node, result);
        }
        return result;
    }

    double scene::last_deltatime() const
    {
        return _last_deltatime;
    }

    void scene::reload_tree_parenting() const
    {
        for (const auto& root_node : _root_nodes)
        {
            reload_node_parenting(root_node, nullptr);
        }
    }

    void scene::init_descriptor_set_layout()
    {
        const auto& [set_index, definition] = descriptor_set_definition();
        _scene_descriptor_set_layout = definition.create_descriptor_set_layout(get_renderer().vkctx());
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