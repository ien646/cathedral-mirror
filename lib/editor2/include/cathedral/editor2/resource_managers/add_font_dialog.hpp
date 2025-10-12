#pragma once

#include <cathedral/core.hpp>

#include <functional>

namespace cathedral::editor2
{
    class add_font_dialog
    {
    public:
        void tick();

        void set_forbidden_names(std::vector<std::string> names);
        void open();

        struct
        {
            std::function<
                void(std::string name, std::string file, std::array<int, 2> atlas_size, int glyph_height, int char_offset)>
                create;
        } callbacks;

    private:
        one_time_flag _open_flag{ false };
        std::vector<std::string> _forbidden_names;

        std::string _name;
        std::string _font_file;
        std::array<int, 2> _atlas_size = { 1024, 1024 };
        int _glyph_height = 64;
        int _char_offset = 0;

        bool validate_fields() const;
    };
} // namespace cathedral::editor2