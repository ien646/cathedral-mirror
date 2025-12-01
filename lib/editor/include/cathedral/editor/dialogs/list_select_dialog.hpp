#pragma once

#include <cathedral/core.hpp>
#include <cathedral/editor/callback_decl.hpp>

namespace cathedral::editor
{
    class list_select_dialog
    {
    public:
        void set_items(std::vector<std::string> items);
        void open();
        void tick();

        CATHEDRAL_DECLARE_CALLBACKS((selected, const std::string&));

    private:
        std::vector<std::string> _items;
        one_time_flag _open_flag{ false };
        std::string _selected;
    };
} // namespace cathedral::editor