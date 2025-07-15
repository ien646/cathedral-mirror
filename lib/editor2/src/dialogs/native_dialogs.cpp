#include "cathedral/bits/error.hpp"

#include <cathedral/editor2/dialogs/native_dialogs.hpp>

#include <ien/str_utils.hpp>

#include <nfd.h>

namespace cathedral::editor2
{
    std::optional<std::string> file_select_dialog(const std::vector<std::pair<std::string, std::string>>& filters)
    {
        NFD_Init();

        nfdu8char_t* out_path = nullptr;
        std::vector<nfdu8filteritem_t> nfd_filters;
        for (const auto& [name, extensions] : filters)
        {
            nfd_filters.push_back({ name.c_str(), extensions.c_str() });
        }
        nfdopendialognargs_t nfdargs = {};
        nfdargs.filterList = nfd_filters.data();
        nfdargs.filterCount = nfd_filters.size();

        const nfdresult_t dialog_result = NFD_OpenDialogU8_With(&out_path, &nfdargs);
        if (dialog_result == NFD_OKAY)
        {
            auto result = ien::xstr_to_str(out_path);
            NFD_FreePathU8(out_path);
            return result;
        }
        if (dialog_result == NFD_CANCEL)
        {
            return std::nullopt;
        }

        CRITICAL_ERROR(std::format("NFD Error: {}", NFD_GetError()));
    }

    std::optional<std::string> directory_select_dialog()
    {
        NFD_Init();

        nfdu8char_t* out_path = nullptr;
        constexpr nfdpickfolderu8args_t nfdargs = {};

        const nfdresult_t dialog_result = NFD_PickFolderU8_With(&out_path, &nfdargs);
        if (dialog_result == NFD_OKAY)
        {
            auto result = ien::xstr_to_str(out_path);
            NFD_FreePathU8(out_path);
            return result;
        }
        if (dialog_result == NFD_CANCEL)
        {
            return std::nullopt;
        }

        CRITICAL_ERROR(std::format("NFD Error: {}", NFD_GetError()));
    }
} // namespace cathedral::editor2