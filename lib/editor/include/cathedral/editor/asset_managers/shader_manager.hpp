#pragma once

#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

namespace Ui
{
    class shader_manager;
}

namespace cathedral::editor
{
    class shader_manager : public QWidget
    {
        Q_OBJECT
        
    public:
        shader_manager();

    private:
        Ui::shader_manager* _ui = nullptr;
    };
}