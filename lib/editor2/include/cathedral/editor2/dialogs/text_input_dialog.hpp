#pragma once

#include <cathedral/editor2/widget.hpp>

#include <functional>
#include <string>
#include <unordered_set>

namespace cathedral::editor2
{
    struct text_input_dialog_callbacks
    {
        std::function<void(std::string)> selected;
    };

    class text_input_dialog final : public widget<text_input_dialog_callbacks>
    {
    public:
        text_input_dialog(
            std::string title,
            std::string label,
            bool allow_empty,
            std::unordered_set<std::string> forbidden_inputs);

        void tick() override;

        void open();

    private:
        bool _open = false;
        bool _first_open = false;
        bool _allow_empty = false;
        std::string _title;
        std::string _label;
        std::unordered_set<std::string> _forbidden_inputs;
        std::string _input;
    };
} // namespace cathedral::editor2