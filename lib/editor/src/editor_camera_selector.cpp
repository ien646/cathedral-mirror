#include <cathedral/editor/editor_camera_selector.hpp>

#include <QHBoxLayout>
#include <QPushButton>

namespace cathedral::editor
{
    constexpr auto STANDARD_BUTTON_STYLESHEET = "";
    constexpr auto SELECTED_BUTTON_STYLESHEET = "QPushButton{background-color:rgb(128,64,128)}";

    editor_camera_selector::editor_camera_selector(QWidget* parent)
        : QWidget(parent)
    {
        auto* main_layout = new QHBoxLayout(this);
        main_layout->setContentsMargins(0, 0, 0, 0);
        setLayout(main_layout);

        _editor_2d_button = new QPushButton(this);
        _editor_2d_button->setText("2D");

        _editor_3d_button = new QPushButton(this);
        _editor_3d_button->setText("3D");

        _game_button = new QPushButton(this);
        _game_button->setText("Game");

        main_layout->addWidget(_editor_2d_button);
        main_layout->addWidget(_editor_3d_button);
        main_layout->addWidget(_game_button);
        main_layout->addStretch();

        connect(_editor_2d_button, &QPushButton::clicked, this, [this] {
            emit editor_2d_selected();
            update_stylesheets(editor_camera_type::EDITOR_2D);
        });
        connect(_editor_3d_button, &QPushButton::clicked, this, [this] {
            emit editor_3d_selected();
            update_stylesheets(editor_camera_type::EDITOR_3D);
        });
        connect(_game_button, &QPushButton::clicked, this, [this] {
            emit game_selected();
            update_stylesheets(editor_camera_type::GAME);
        });
    }

    void editor_camera_selector::set_current_camera(const editor_camera_type type)
    {
        _current_camera = type;
        switch (type)
        {
        case editor_camera_type::EDITOR_2D:
            emit editor_2d_selected();
            break;
        case editor_camera_type::EDITOR_3D:
            emit editor_3d_selected();
            break;
        case editor_camera_type::GAME:
            emit game_selected();
            break;
        default:
            CRITICAL_ERROR("Invalid editor camera type");
            break;
        }
        update_stylesheets(type);
    }

    editor_camera_type editor_camera_selector::current_camera() const
    {
        return _current_camera;
    }

    void editor_camera_selector::update_stylesheets(const editor_camera_type selected_type) const
    {
        _editor_2d_button->setStyleSheet(STANDARD_BUTTON_STYLESHEET);
        _editor_3d_button->setStyleSheet(STANDARD_BUTTON_STYLESHEET);
        _game_button->setStyleSheet(STANDARD_BUTTON_STYLESHEET);
        switch (selected_type)
        {
        case editor_camera_type::EDITOR_2D:
            _editor_2d_button->setStyleSheet(SELECTED_BUTTON_STYLESHEET);
            break;
        case editor_camera_type::EDITOR_3D:
            _editor_3d_button->setStyleSheet(SELECTED_BUTTON_STYLESHEET);
            break;
        case editor_camera_type::GAME:
            _game_button->setStyleSheet(SELECTED_BUTTON_STYLESHEET);
            break;
        default:
            CRITICAL_ERROR("Invalid case");
        }
    }
} // namespace cathedral::editor