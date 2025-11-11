#include <cathedral/script/enums.hpp>

#include <cathedral/script/init_macros.hpp>
#include <cathedral/script/state.hpp>

#include <cathedral/engine/font_mode.hpp>
#include <cathedral/engine/input.hpp>
#include <cathedral/engine/material_domain.hpp>
#include <cathedral/engine/node_type.hpp>

#include <ien/str_utils.hpp>

#include <magic_enum.hpp>

namespace cathedral::script
{
    namespace
    {
        template <typename TEnum>
        std::string generate_enum_annotations(const std::string& enum_name)
        {
            std::string result = std::format("---@enum (key) {} \n", enum_name);
            result += std::format("{} = {{ \n", enum_name);
            for (const auto& [value, name] : magic_enum::enum_entries<TEnum>())
            {
                result += std::format("\t{} = {},\n", name, static_cast<int>(value));
            }
            result += "}\n\n";
            return result;
        }
    } // namespace

    void enums_initializer::initialize(state& s)
    {
        AUTO_INIT_ENUM(s, engine, keyboard_keycode);
        AUTO_INIT_ENUM(s, engine, mouse_button);
        AUTO_INIT_ENUM(s, engine, material_domain);
        AUTO_INIT_ENUM(s, engine, font_mode);
    }

    const std::string& enums_initializer::get_annotations()
    {
        static const std::string annotations = [] {
            std::string result;
            result += generate_enum_annotations<engine::keyboard_keycode>("keyboard_keycode");
            result += generate_enum_annotations<engine::mouse_button>("mouse_button");
            result += generate_enum_annotations<engine::material_domain>("material_domain");
            result += generate_enum_annotations<engine::font_mode>("font_mode");
            return result;
        }();
        return annotations;
    }
} // namespace cathedral::script