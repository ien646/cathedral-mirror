#pragma once

#include <cathedral/editor2/widget.hpp>
#include <functional>

namespace cathedral::editor2
{
    struct confirm_dialog_callbacks
    {
        std::function<void(bool yes)> selected;
    };

    class confirm_dialog : public widget<confirm_dialog_callbacks>
    {
    public:
        void set_title(std::string title);
        void set_message(std::string message);
        void open();

        void tick() override;

    private:
        std::string _title;
        std::string _message;
        bool _open = false;
        bool _first_open = false;
    };
} // namespace cathedral::editor2