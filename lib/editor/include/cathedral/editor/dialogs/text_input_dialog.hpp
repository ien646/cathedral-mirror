#pragma once

#include <cathedral/editor/callback_decl.hpp>

#include <string>

namespace cathedral::editor
{
    class text_input_dialog
    {
    public:
        explicit text_input_dialog(std::string title, std::string label, size_t initial_length = 255);

        void set_buffer_size(size_t length = 255);

        void set_text(const std::string& text);
        std::string text() const;

        void set_validator(std::function<bool(const std::string&)> validator);
        void open();

        void tick();

        CATHEDRAL_DECLARE_CALLBACKS((accepted, ), (cancelled, ));

    private:
        bool _open_flag = false;
        std::function<bool(const std::string&)> _validator;
        std::string _title;
        std::string _label;
        std::string _buffer;
    };
} // namespace cathedral::editor