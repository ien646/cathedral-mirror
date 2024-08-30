#include <cathedral/editor/common/float_edit.hpp>

#include <QDoubleValidator>

namespace cathedral::editor
{
    float_edit::float_edit(QWidget* parent, unsigned int decimal_digits)
        : QLineEdit(parent)
    {
        auto* validator =
            new QDoubleValidator(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max(), decimal_digits, this);
        validator->setDecimals(decimal_digits);
        validator->setNotation(QDoubleValidator::Notation::StandardNotation);
        validator->setLocale(QLocale::system());

        setValidator(validator);
    }

    void float_edit::set_value(float v)
    {
        setText(QString::number(v));
    }

    float float_edit::get_value() const
    {
        bool ok = false;
        float v = text().toFloat(&ok);
        return ok ? v : NAN;
    }
} // namespace cathedral::editor