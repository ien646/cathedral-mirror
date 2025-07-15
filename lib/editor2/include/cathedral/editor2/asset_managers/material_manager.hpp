#pragma once

#include <cathedral/core.hpp>

#include <cathedral/engine/material.hpp>

#include <cathedral/editor2/dialogs/confirm_dialog.hpp>
#include <cathedral/editor2/dialogs/text_input_dialog.hpp>
#include <cathedral/editor2/widget.hpp>

#include <memory>

FORWARD_CLASS(cathedral::project, project);

namespace cathedral::editor2
{
    class material_manager final : public widget<>
    {
    public:
        explicit material_manager(project::project& project);
        void tick() override;

        void open();

    private:
        project::project& _project;
        bool _open = false;
        bool _first_open = false;
        std::optional<std::string> _selected_material = std::nullopt;
        std::unique_ptr<text_input_dialog> _new_material_dialog;
        std::unique_ptr<text_input_dialog> _rename_material_dialog;
        std::unique_ptr<confirm_dialog> _delete_material_dialog;

        std::unordered_map<std::string, engine::material> _dummy_materials;

        void tick_material_list();
        void tick_shader_combos() const;
        void tick_material_vars();
    };
} // namespace cathedral::editor2