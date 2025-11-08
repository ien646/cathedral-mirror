#pragma once

#include <cathedral/editor2/callback_decl.hpp>

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

        CATHEDRAL_DECLARE_CALLBACKS((accepted, void), (cancelled, void));

    private:
        bool _open_flag = false;
        std::string _title;
        std::string _label;
    };
} // namespace cathedral::editor2