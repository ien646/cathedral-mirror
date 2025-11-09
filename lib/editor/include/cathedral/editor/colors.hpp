#pragma once

#include <imgui.h>

namespace cathedral::editor
{
    namespace colors
    {
        constexpr ImVec4 BG_ERROR_RED = { 1.0F, 0.1F, 0.1F, 1.0F };
        constexpr ImVec4 BG_WARNING_YELLOW = { 1.0F, 1.0F, 0.1F, 1.0F };
        constexpr ImVec4 BG_INFO_BLUE = { 0.1F, 0.1F, 1.0F, 1.0F };

        constexpr ImVec4 TEXT_ERROR_RED = { 1.0F, 0.5F, 0.5F, 1.0F };
        constexpr ImVec4 TEXT_WARNING_YELLOW = { 1.0F, 1.0F, 0.2F, 1.0F };
        constexpr ImVec4 TEXT_INFO_BLUE = { 0.5F, 0.5F, 1.0F, 1.0F };
    } // namespace colors
} // namespace cathedral::editor