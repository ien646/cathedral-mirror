#include <cathedral/editor/editor_gamemode.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

namespace cathedral::editor
{
    editor_gamemode::editor_gamemode(QWidget* parent)
        : QWidget(parent)
    {
        auto* layout = new QHBoxLayout;
        setLayout(layout);

        auto* play_button = new QPushButton("Play");
        play_button->setIcon(QIcon(":/icons/play_icon.png"));

        layout->addWidget(play_button);

        connect(play_button, &QPushButton::clicked, this, &editor_gamemode::play_clicked);
    }
} // namespace cathedral::editor