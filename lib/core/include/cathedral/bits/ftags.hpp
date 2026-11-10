#pragma once

namespace cathedral
{
    inline constexpr struct _export_script_t
    {
        constexpr bool operator==(const _export_script_t& other) const = default;
    } export_script;
} // namespace cathedral

#define EXPORT_SCRIPT [[= cathedral::export_script]]