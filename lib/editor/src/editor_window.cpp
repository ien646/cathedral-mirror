#include <cathedral/editor/editor_window.hpp>

#include <cathedral/editor/asset_managers/material_definition_manager.hpp>
#include <cathedral/editor/asset_managers/material_manager.hpp>
#include <cathedral/editor/asset_managers/mesh_manager.hpp>
#include <cathedral/editor/asset_managers/shader_manager.hpp>
#include <cathedral/editor/asset_managers/texture_manager.hpp>

#include <cathedral/editor/common/message.hpp>
#include <cathedral/editor/common/text_input_dialog.hpp>

#include <cathedral/engine/nodes/camera3d_node.hpp>
#include <cathedral/engine/nodes/mesh3d_node.hpp>

#include <QDockWidget>
#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QMouseEvent>
#include <QTimer>
#include <QVBoxLayout>
#include <QVulkanInstance>
#include <QWindow>

#include <ien/fs_utils.hpp>
#include <ien/platform.hpp>

// clang-format off
#if defined(IEN_OS_WIN)
    #include <ien/win32/windows.h>
    #include <vulkan/vulkan_win32.h>
    std::vector<const char*> get_instance_extensions()
    {
        return { VK_KHR_WIN32_SURFACE_EXTENSION_NAME };
    };
#elif defined(IEN_OS_LINUX)
    #include <vulkan/vulkan_wayland.h>
    #include <xcb/xcb.h>
    #include <vulkan/vulkan_xcb.h>
    
    std::vector<const char*> get_instance_extensions() 
    {
        if(qgetenv("QT_QPA_PLATFORM") == "xcb")
        {
            return {VK_KHR_XCB_SURFACE_EXTENSION_NAME};
        }
        return {VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME};
    };
#endif
// clang-format on

namespace cathedral::editor
{
    editor_window::editor_window(std::shared_ptr<project::project> project)
        : _project(std::move(project))
    {
        _menubar = new editor_window_menubar(this);
        setMenuBar(_menubar);

        _scene_dock = new scene_dock_widget(this);
        _scene_dock->setAllowedAreas(Qt::DockWidgetArea::AllDockWidgetAreas);
        _scene_dock->setFeatures(QDockWidget::DockWidgetFeature::DockWidgetMovable);
        _scene_dock->setMinimumWidth(200);
        addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, _scene_dock);

        _props_dock = new properties_dock_widget(_project.get(), *_scene, this);
        _props_dock->setAllowedAreas(Qt::DockWidgetArea::AllDockWidgetAreas);
        _props_dock->setFeatures(QDockWidget::DockWidgetFeature::DockWidgetMovable);
        _props_dock->setMinimumWidth(200);
        addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, _props_dock);

        connect(_scene_dock, &scene_dock_widget::node_selected, this, [this](engine::scene_node* node) {
            if (auto* mesh = dynamic_cast<engine::mesh3d_node*>(node))
            {
                _props_dock->set_node(mesh);
            }
            else if (auto* camera = dynamic_cast<engine::camera3d_node*>(node))
            {
                _props_dock->set_node(camera);
            }
            else if (auto* node3d = dynamic_cast<engine::node*>(node))
            {
                _props_dock->set_node(node3d);
            }
            else
            {
                _props_dock->clear_node();
            }
        });

        setup_menubar_connections();
    }

    void editor_window::tick(const std::function<void(double)>& tick_work)
    {
        _scene->tick(tick_work);
        update();
    }

    void editor_window::initialize_vulkan()
    {
        _vulkan_widget = std::make_unique<vulkan_widget>(QGuiApplication::topLevelWindows().at(0), this);

        setCentralWidget(_vulkan_widget->get_widget());

        cathedral::gfx::vulkan_context_args vkctx_args;
        vkctx_args.instance_extensions = get_instance_extensions();
        vkctx_args.surface_retriever = [this](vk::Instance inst) { return _vulkan_widget->init_surface(inst); };
        vkctx_args.surface_size_retriever = [this]() {
            const auto* widget = _vulkan_widget->get_widget();
            return glm::ivec2{ widget->width() * devicePixelRatio(), widget->height() * devicePixelRatio() };
        };
        vkctx_args.validation_layers = is_debug_build();

        _vkctx = std::make_unique<cathedral::gfx::vulkan_context>(vkctx_args);
        _swapchain = std::make_unique<cathedral::gfx::swapchain>(*_vkctx, vk::PresentModeKHR::eFifo);

        engine::renderer_args renderer_args;
        renderer_args.swapchain = &*_swapchain;
        _renderer = std::make_unique<engine::renderer>(renderer_args);

        engine::scene_args scene_args;
        scene_args.prenderer = _renderer.get();
        scene_args.loaders = _project->get_loader_funcs();
        _scene = std::make_unique<engine::scene>(std::move(scene_args));

        _scene_dock->set_scene(_scene.get());

        auto* timer = new QTimer(this);
        timer->setSingleShot(true);
        timer->setInterval(500);
        timer->start();
        connect(timer, &QTimer::timeout, this, [this, timer] {
            resize(800, 600);
            timer->deleteLater();
        });

        connect(
            _vulkan_widget.get(),
            &vulkan_widget::size_changed,
            this,
            [this]([[maybe_unused]] int w, [[maybe_unused]] int h) {
                _swapchain->recreate();
                _renderer->recreate_swapchain_dependent_resources();
            });
    }

    void editor_window::setup_menubar_connections()
    {
        connect(_menubar, &editor_window_menubar::open_project_clicked, this, [this] { open_project(); });
        connect(_menubar, &editor_window_menubar::close_clicked, this, &editor_window::close);

        connect(_menubar, &editor_window_menubar::texture_manager_clicked, this, [this] { open_texture_manager(); });
        connect(_menubar, &editor_window_menubar::shader_manager_clicked, this, [this] { open_shader_manager(); });
        connect(_menubar, &editor_window_menubar::material_manager_clicked, this, [this] { open_material_manager(); });
        connect(_menubar, &editor_window_menubar::material_definition_manager_clicked, this, [this] {
            open_material_definition_manager();
        });
        connect(_menubar, &editor_window_menubar::mesh_manager_clicked, this, [this] { open_mesh_manager(); });

        connect(_menubar, &editor_window_menubar::new_scene_clicked, this, [this] { new_scene(); });
        connect(_menubar, &editor_window_menubar::open_scene_clicked, this, [this] { open_scene(); });
        connect(_menubar, &editor_window_menubar::save_scene_clicked, this, [this] { save_scene(); });
    }

    void editor_window::open_project()
    {
        const QString dir = QFileDialog::getExistingDirectory(
            this,
            "Select project directory",
            QString::fromStdString(ien::get_current_user_homedir()));

        if (dir.isEmpty())
        {
            return;
        }

        if (_project->load_project(dir.toStdString()) != project::load_project_status::OK)
        {
            show_error_message("Failure loading project");
        }

        // Preload all shaders (for now?)
        for (const auto& [name, asset] : _project->shader_assets())
        {
            switch (asset->type())
            {
            case cathedral::gfx::shader_type::VERTEX:
                std::ignore = _renderer->create_vertex_shader(name, asset->source());
                break;
            case cathedral::gfx::shader_type::FRAGMENT:
                std::ignore = _renderer->create_fragment_shader(name, asset->source());
                break;
            case cathedral::gfx::shader_type::UNDEFINED:
            default:
                CRITICAL_ERROR("Unhandled shader type");
            }
        }
    }

    void editor_window::open_material_manager()
    {
        _material_manager = new material_manager(_project.get(), *_scene, this);
        _material_manager->setWindowModality(Qt::WindowModality::WindowModal);
        _material_manager->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);
        _material_manager->show();
    }

    void editor_window::open_material_definition_manager()
    {
        _material_definition_manager = new material_definition_manager(_project.get(), this);
        _material_definition_manager->setWindowModality(Qt::WindowModality::WindowModal);
        _material_definition_manager->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);
        _material_definition_manager->show();
    }

    void editor_window::open_mesh_manager()
    {
        _mesh_manager = new mesh_manager(_project.get(), this);
        _mesh_manager->setWindowModality(Qt::WindowModality::WindowModal);
        _mesh_manager->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);
        _mesh_manager->show();
    }

    void editor_window::open_shader_manager()
    {
        _shader_manager = new shader_manager(_project.get(), *_scene, this);
        _shader_manager->setWindowModality(Qt::WindowModality::WindowModal);
        _shader_manager->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);
        _shader_manager->show();
    }

    void editor_window::open_texture_manager()
    {
        _texture_manager = new texture_manager(_project.get(), this);
        _texture_manager->setWindowModality(Qt::WindowModality::WindowModal);
        _texture_manager->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);
        _texture_manager->show();
    }

    void editor_window::new_scene()
    {
        if (!show_confirm_dialog("Unsaved changes will be lost. Continue?", this))
        {
            auto* input_dialog = new text_input_dialog(this, "New scene", "Name:", false, "new_scene");
            if (input_dialog->exec() != QDialog::Accepted)
            {
                return;
            }

            const auto& scene_name = input_dialog->result().toStdString();
            const auto abs_path = (std::filesystem::path(_project->scenes_path()) / (scene_name + ".cscene")).string();
            if (std::filesystem::exists(abs_path))
            {
                show_error_message("A scene with that name already exists");
                return;
            }

            engine::scene_args scene_args;
            scene_args.loaders = _project->get_loader_funcs();
            scene_args.prenderer = _renderer.get();

            _scene = std::make_unique<engine::scene>(std::move(scene_args));
            setWindowTitle(QString::fromStdString(scene_name));
        }
    }

    void editor_window::open_scene()
    {
        if (!show_confirm_dialog("Unsaved changes will be lost. Continue?", this))
        {
            show_error_message("not implemented", this);
        }
    }

    void editor_window::save_scene()
    {
        auto* input = new text_input_dialog(this, "Choose a scene name", "Name: ", false);
        if (input->exec() == QDialog::Accepted)
        {
            _project->save_scene(*_scene, input->result().toStdString() + ".cscene");
        }
    }
} // namespace cathedral::editor