#include <cathedral/editor/editor_gamemode.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

namespace cathedral::editor
{
    editor_gamemode::editor_gamemode(QWidget* parent)
        : QWidget(parent)
    {
        setContentsMargins(0, 0, 0, 0);

        auto* layout = new QHBoxLayout;
        layout->setContentsMargins(0, 0, 0, 0);
        setLayout(layout);

        auto* play_button = new QPushButton("Play");
        play_button->setIcon(QIcon(":/icons/play_icon.png"));

        layout->addWidget(play_button);

        connect(play_button, &QPushButton::clicked, this, &editor_gamemode::play_clicked);
    }
} // namespace cathedral::editor