#include <cathedral/engine/scene.hpp>

#include <cathedral/engine/nodes/camera2d_node.hpp>
#include <cathedral/engine/nodes/camera3d_node.hpp>
#include <cathedral/engine/nodes/directional_light_node.hpp>
#include <cathedral/engine/nodes/mesh3d_node.hpp>
#include <cathedral/engine/nodes/node.hpp>
#include <cathedral/engine/nodes/point_light_node.hpp>
#include <cathedral/engine/nodes/text_node.hpp>

#include <ien/algorithm.hpp>

#include <print>
#include <ranges>

namespace cathedral::engine
{
    constexpr auto SCENE_GLSL_STR_FORMAT = R"glsl(
struct scene_point_light
{{
    vec3 position;
    float intensity;
    vec3 color;
    float range;
    float falloff_coefficient;
}};

struct scene_directional_light
{{
    vec3 direction;
    float intensity;
    vec3 color;
}};

layout(set = 0, binding = 0) uniform _scene_uniform_data_ {{
    float deltatime;
    uint frame_index;
    uint enabled_point_lights;
    uint enabled_directional_lights;
    vec3 ambient_light;
    uint viewport_width;
    uint viewport_height;
    mat4 projection2d;
    mat4 projection3d;
    mat4 view2d;
    mat4 view3d;
    scene_directional_light directional_lights[{0}];
    scene_point_light point_lights[{1}];
}} scene_uniform_data;

#define DELTATIME scene_uniform_data.deltatime
#define FRAME_INDEX scene_uniform_data.frame_index
#define PROJECTION_2D scene_uniform_data.projection2d
#define PROJECTION_3D scene_uniform_data.projection3d
#define VIEW_2D scene_uniform_data.view2d
#define VIEW_3D scene_uniform_data.view3d
#define AMBIENT_LIGHT scene_uniform_data.ambient_light
#define VIEWPORT_WIDTH scene_uniform_data.viewport_width
#define VIEWPORT_HEIGHT scene_uniform_data.viewport_height
#define DIRECTIONAL_LIGHTS scene_uniform_data.directional_lights
#define ENABLED_DIRECTIONAL_LIGHTS scene_uniform_data.enabled_directional_lights
#define POINT_LIGHTS scene_uniform_data.point_lights
#define ENABLED_POINT_LIGHTS scene_uniform_data.enabled_point_lights
)glsl";

    std::string get_scene_uniform_glslstr()
    {
        return std::format(SCENE_GLSL_STR_FORMAT, CATHEDRAL_SCENE_MAX_DIRECTIONAL_LIGHTS, CATHEDRAL_SCENE_MAX_POINT_LIGHTS);
    }

    scene::scene(scene_args args)
        : _args(std::move(args))
        , _mesh_buffer_storage(_args.prenderer)
    {
        CRITICAL_CHECK_NOTNULL(_args.loaders.font_loader);
        CRITICAL_CHECK_NOTNULL(_args.loaders.material_loader);
        CRITICAL_CHECK_NOTNULL(_args.loaders.mesh_loader);
        CRITICAL_CHECK_NOTNULL(_args.loaders.script_loader);
        CRITICAL_CHECK_NOTNULL(_args.loaders.texture_loader);

        CRITICAL_CHECK(!_args.name.empty(), "Scene name cannot be empty");

        gfx::uniform_buffer_args uniform_buffer_args;
        uniform_buffer_args.size = sizeof(scene_uniform_data);
        uniform_buffer_args.vkctx = &get_renderer().vkctx();

        _uniform_buffer = std::make_unique<gfx::uniform_buffer>(uniform_buffer_args);

        init_descriptor_set_layout();
        init_descriptor_set();

        _previous_frame_timepoint = scene_clock::now();

        _debug_line_renderer = std::make_unique<debug::line_renderer>(*this);
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
        _used_directional_lights = 0;

        get_renderer().begin_frame([this] {
            _debug_line_renderer->pre_render_tick();
        });

        func(deltatime_s);
        _last_deltatime = deltatime_s;

        const auto vp_size = get_renderer().vkctx().get_surface_size();

        _scene_uniform_data.deltatime = static_cast<float>(deltatime_s);
        _scene_uniform_data.frame_index = static_cast<uint32_t>(get_renderer().current_frame());
        _scene_uniform_data.viewport_width = vp_size.x;
        _scene_uniform_data.viewport_height = vp_size.y;
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
        _scene_uniform_data.enabled_directional_lights = _used_directional_lights;

        _debug_line_renderer->render_tick(deltatime_s);

        get_renderer().end_frame();

        if (_keyboard_input != nullptr)
        {
            _keyboard_input->tick();
        }
        if (_mouse_input != nullptr)
        {
            _mouse_input->tick();
        }
    }

    scene_node* scene::add_root_node(const std::string& name, const node_type type)
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
        case node_type::DIRECTIONAL_LIGHT:
            return add_root_node<directional_light_node>(name);
        case node_type::TEXT_NODE:
            return add_root_node<text_node>(name);
        default:
            CRITICAL_ERROR("Unhandled node type");
        }
    }

    void scene::add_root_node(std::unique_ptr<scene_node>&& node)
    {
        _root_nodes.push_back(std::move(node));
    }

    scene_node* scene::get_node(const std::string& name)
    {
        const auto it = std::ranges::find_if(_root_nodes, [&name](const auto& node) { return node->name() == name; });

        if (it != _root_nodes.end())
        {
            return it->get();
        }
        return nullptr;
    }

    void scene::remove_node(const std::string& name)
    {
        const auto it = std::ranges::find_if(_root_nodes, [&name](const auto& node) { return node->name() == name; });

        CRITICAL_CHECK(it != _root_nodes.end(), "Node not found");
        ien::erase_unsorted(_root_nodes, it);
    }

    std::unique_ptr<scene_node> scene::detach_node(const std::string& name)
    {
        const auto it = std::ranges::find_if(_root_nodes, [&name](const auto& node) { return node->name() == name; });
        if (it == _root_nodes.end())
        {
            return {};
        }
        std::unique_ptr<scene_node> result(it->release());
        ien::erase_unsorted(_root_nodes, it);
        return result;
    }

    bool scene::contains_node(const std::string& name) const
    {
        return std::ranges::find_if(_root_nodes, [&name](const auto& node) { return node->name() == name; }) !=
               _root_nodes.end();
    }

    const std::vector<std::unique_ptr<scene_node>>& scene::root_nodes() const
    {
        return _root_nodes;
    }

    void scene::update_uniform(const std::function<void(scene_uniform_data&)>& func)
    {
        func(_scene_uniform_data);
    }

    std::shared_ptr<mesh_buffer> scene::get_mesh_buffers(const std::string& mesh_path, const mesh& mesh)
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

    std::shared_ptr<script> scene::load_script(const std::string& name)
    {
        return _args.loaders.script_loader(name, *this);
    }

    std::shared_ptr<font> scene::load_font(const std::string& name)
    {
        return _args.loaders.font_loader(name, *this);
    }

    void scene::load_nodes(std::vector<std::unique_ptr<scene_node>>&& root_nodes)
    {
        _args.prenderer->vkctx().device().waitIdle();
        _root_nodes = std::move(root_nodes);
    }

    void scene::set_frame_point_light(const point_light_data& data)
    {
        if (_used_point_lights < CATHEDRAL_SCENE_MAX_POINT_LIGHTS)
        {
            _scene_uniform_data.point_lights[_used_point_lights++] = data;
        }
        else
        {
            log_warning(
                std::format("Reached max limit of active non-culled point lights ({})", CATHEDRAL_SCENE_MAX_POINT_LIGHTS));
        }
    }

    void scene::set_frame_directional_light(const directional_light_data& data)
    {
        if (_used_directional_lights < CATHEDRAL_SCENE_MAX_DIRECTIONAL_LIGHTS)
        {
            _scene_uniform_data.directional_lights[_used_directional_lights++] = data;
        }
        else
        {
            log_warning(
                std::format("Reached max limit of active directional lights ({})", CATHEDRAL_SCENE_MAX_DIRECTIONAL_LIGHTS));
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
        void get_nodes_of_type(const node_type type, scene_node* node, std::vector<scene_node*>& target)
        {
            if (node->type() == type)
            {
                target.push_back(node);
            }
            for (const auto& child : node->children())
            {
                get_nodes_of_type(type, child.get(), target);
            }
        }
    } // namespace

    std::vector<scene_node*> scene::get_nodes_by_type(const node_type type) const
    {
        std::vector<scene_node*> result;
        for (auto& node : _root_nodes)
        {
            get_nodes_of_type(type, node.get(), result);
        }
        return result;
    }

    double scene::last_deltatime() const
    {
        return _last_deltatime;
    }

    void scene::set_main_camera_3d_node(camera3d_node* node)
    {
        _main_camera_3d = node;
    }

    camera3d_node* scene::main_camera_3d_node() const
    {
        return _main_camera_3d;
    }

    void scene::draw_debug_line(std::vector<debug::line_vertex> vertices, const double lifetime_seconds) const
    {
        _debug_line_renderer->add_line(std::move(vertices), lifetime_seconds);
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