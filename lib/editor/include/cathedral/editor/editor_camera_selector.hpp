#pragma once

#include <cathedral/core.hpp>

#include <QWidget>

FORWARD_CLASS_INLINE(QPushButton);

namespace cathedral::editor
{
    enum class editor_camera_type : uint8_t
    {
        EDITOR_2D,
        EDITOR_3D,
        GAME
    };

    class editor_camera_selector final : public QWidget
    {
        Q_OBJECT

    public:
        explicit editor_camera_selector(QWidget* parent = nullptr);

        void set_current_camera(editor_camera_type type);
        editor_camera_type current_camera() const;

    signals:
        void editor_2d_selected();
        void editor_3d_selected();
        void game_selected();

    private:
        QPushButton* _editor_2d_button = nullptr;
        QPushButton* _editor_3d_button = nullptr;
        QPushButton* _game_button = nullptr;
        editor_camera_type _current_camera = editor_camera_type::EDITOR_3D;

        void update_stylesheets(editor_camera_type selected_type) const;
    };
} // namespace cathedral::editor