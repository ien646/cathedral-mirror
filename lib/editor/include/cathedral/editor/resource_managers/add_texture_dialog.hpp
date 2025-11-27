#pragma once

#include <cathedral/engine/texture_format.hpp>

#include <cathedral/core.hpp>
#include <cathedral/editor/callback_decl.hpp>

#include <ien/image/resize_filter.hpp>

#include <functional>

namespace cathedral::editor
{
    class add_texture_dialog
    {
    public:
        void tick();

        void set_forbidden_names(std::vector<std::string> names);
        void open();

        CATHEDRAL_DECLARE_CALLBACKS(
            (create,
             std::string name,
             std::string file,
             engine::texture_format format,
             uint8_t mip_count,
             ien::resize_filter mipgen_filter));

    private:
        one_time_flag _open_flag{ false };
        std::vector<std::string> _forbidden_names;

        std::string _name;
        std::string _texture_file;
        engine::texture_format _texture_format = engine::texture_format::R8G8B8A8_SRGB;
        ien::resize_filter _mipgen_filter = ien::resize_filter::DEFAULT;
        int _mip_count = 1;

        bool validate_fields() const;
    };
} // namespace cathedral::editor