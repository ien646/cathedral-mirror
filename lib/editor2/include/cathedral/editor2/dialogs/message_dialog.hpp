#pragma once

#include <string>

namespace cathedral::editor2
{
    enum class message_dialog_mode
    {
        INFO,
        WARNING,
        ERROR
    };

    class message_dialog
    {
    public:
        void set_title(std::string text);
        void set_text(std::string text);
        void set_mode(message_dialog_mode mode);

        void open();
        void tick();

    private:
        bool _open_flag = false;
        message_dialog_mode _mode = message_dialog_mode::INFO;
        std::string _title = "message";
        std::string _label = "placeholder";
    };
} // namespace cathedral::editor2