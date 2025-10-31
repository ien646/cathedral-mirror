#pragma once

#include <cathedral/core.hpp>
#include <cathedral/editor2/widgets/texture_widget.hpp>

#include <functional>

FORWARD_CLASS(cathedral::engine, scene);

namespace cathedral::editor2
{
    class texture_selector
    {
    public:
        texture_selector(engine::scene& scene);
        void set_texture_list(std::vector<std::string> names);

        void open();

        void tick();

        struct
        {
            std::function<void(const std::string&)> selected;
        } callbacks;

    private:
        engine::scene& _scene;
        one_time_flag _open_flag;
        std::vector<std::string> _texture_list;
        std::vector<texture_widget> _texture_widgets;
        std::string _filter;
        std::string _selected;
    };
} // namespace cathedral::editor2