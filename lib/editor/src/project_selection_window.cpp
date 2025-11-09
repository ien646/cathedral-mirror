#include <cathedral/editor/project_selection_window.hpp>

#include <cathedral/editor/native/file_dialog.hpp>
#include <ien/io_utils.hpp>

#include <filesystem>

namespace cathedral::editor
{
    project_selection_window::project_selection_window()
        : _window("Projects", 400, 200, std::make_shared<settings>())
    {
        engine::scene_args scene_args;
        scene_args.name = "project select";
        scene_args.loaders.font_loader = [](auto&&, auto&&) { return std::shared_ptr<engine::font>{}; };
        scene_args.loaders.material_loader = [](auto&&, auto&&) { return std::weak_ptr<engine::material>{}; };
        scene_args.loaders.mesh_loader = [](auto&&, auto&&) { return std::shared_ptr<engine::mesh>{}; };
        scene_args.loaders.script_loader = [](auto&&, auto&&) { return std::shared_ptr<engine::script>{}; };
        scene_args.loaders.texture_loader = [](auto&&, auto&&) { return std::shared_ptr<engine::texture>{}; };
        scene_args.prenderer = &_window.renderer();

        _scene = std::make_unique<engine::scene>(MOVE(scene_args));
    }

    std::optional<std::string> project_selection_window::execute()
    {
        std::string result;
        while (result.empty() && _window.keep_open())
        {
            _scene->tick([&]([[maybe_unused]] const double deltatime) {
                _window.tick([&] {
                    const ImGuiViewport* vp = ImGui::GetMainViewport();
                    ImGui::SetNextWindowPos(vp->WorkPos);
                    ImGui::SetNextWindowSize(vp->WorkSize);
                    ImGui::Begin(
                        "#project_selection_window",
                        nullptr,
                        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);
                    {
                        if (ImGui::BeginTable(
                                "#buttons_table",
                                2,
                                ImGuiTableFlags_SizingStretchSame,
                                ImGui::GetContentRegionAvail()))
                        {
                            ImGui::TableNextColumn();
                            if (ImGui::Button("New Project", ImGui::GetContentRegionAvail()))
                            {
                                if (const auto dir = native_open_dir())
                                {
                                    const std::string text;
                                    ien::write_file_text((std::filesystem::path(*dir) / ".cathedral").string(), text);
                                    result = *dir;
                                }
                            }
                            ImGui::TableNextColumn();
                            if (ImGui::Button("Open Project", ImGui::GetContentRegionAvail()))
                            {
                                if (const auto dir = native_open_dir())
                                {
                                    result = *dir;
                                }
                            }
                            ImGui::EndTable();
                        }
                    }
                    ImGui::End();
                });
            });
        }
        return _window.keep_open() ? std::optional{ result } : std::nullopt;
    }
} // namespace cathedral::editor