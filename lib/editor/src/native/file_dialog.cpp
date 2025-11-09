#include <cathedral/editor/native/file_dialog.hpp>

#include <cathedral/core.hpp>

#include <nfd.h>

#include <chrono>

namespace cathedral::editor
{
    template <typename TCall>
    std::optional<std::string> nfd_open_rigamarole(const TCall& call)
    {
        const auto init_result = NFD_Init();
        if (init_result != NFD_OKAY)
        {
            CRITICAL_ERROR(std::format("Failure initial NFD native file dialog: {}", NFD_GetError()));
        }

        nfdu8char_t* out_path = nullptr;
        const auto dialog_result = call(&out_path);

        if (dialog_result == NFD_OKAY)
        {
            auto result = std::string{ out_path };
            NFD_FreePathU8(out_path);
            NFD_Quit();
            return result;
        }
        if (dialog_result == NFD_CANCEL)
        {
            NFD_Quit();
            return {};
        }
        CRITICAL_ERROR(std::format("Failure opening native file picker dialog: {}", NFD_GetError()));
    }

    std::optional<std::string> native_open_file()
    {
        return nfd_open_rigamarole([](nfdu8char_t** out_path) { return NFD_OpenDialogU8(out_path, nullptr, 0, nullptr); });
    }

    std::optional<std::string> native_open_dir()
    {
        return nfd_open_rigamarole([](nfdu8char_t** out_path) { return NFD_PickFolderU8(out_path, nullptr); });
    }

    std::optional<std::string> native_save_file(const std::optional<std::string>& filename)
    {
        return nfd_open_rigamarole([filename](nfdu8char_t** out_path) {
            return NFD_SaveDialogU8(out_path, nullptr, 0, nullptr, filename ? filename->c_str() : "");
        });
    } // namespace cathedral::editor
} // namespace cathedral::editor