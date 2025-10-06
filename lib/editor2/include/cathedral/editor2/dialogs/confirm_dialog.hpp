#pragma once

#include <functional>
#include <string>

namespace cathedral::editor2
{
    class confirm_dialog
    {
    public:
        explicit confirm_dialog(std::string title, std::string label);

        void set_label(std::string text);

        void open();

        void tick();

        struct
        {
            std::function<void()> accepted;
            std::function<void()> cancelled;
        } callbacks;

    private:
        bool _open_flag = false;
        std::string _title;
        std::string _label;
    };
}