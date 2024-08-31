#pragma once

#include <QLineEdit>

namespace cathedral::editor
{
    class float_edit : public QLineEdit
    {
        Q_OBJECT
        
    public:
        float_edit(QWidget* parent, unsigned int decimal_digits = 8);

        void set_value(float v);
        float get_value() const;
    };
}