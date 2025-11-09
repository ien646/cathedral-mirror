#pragma once

#include <cathedral/core.hpp>
#include <cathedral/editor/engine_window.hpp>
#include <cathedral/engine/scene.hpp>
#include <cathedral/project/project.hpp>

namespace cathedral::editor
{
    CATHEDRAL_ABSTRACT_CLASS resource_manager_base
    {
    public:
        explicit resource_manager_base(project::project & pro)
            : _project(pro)
            , _window("placeholder", 1000, 800, pro.get_settings())
        {
            engine::scene_args args;
            args.name = "resource_manager";
            args.loaders = _project.get_loader_funcs();
            args.prenderer = &_window.renderer();

            _scene = std::make_unique<engine::scene>(MOVE(args));
        }

        virtual ~resource_manager_base() = default;

        bool must_close() const
        {
            return !_window.keep_open();
        }

        virtual void tick() = 0;

    protected:
        project::project& _project;
        engine_window _window;
        std::unique_ptr<engine::scene> _scene;
    };
} // namespace cathedral::editor