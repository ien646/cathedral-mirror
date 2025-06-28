#include <cathedral/editor/common/double_vector.hpp>

namespace cathedral::editor
{
    double_vector2::double_vector2(QWidget* parent)
        : numeric_vector(parent)
    {
        for (size_t i = 0; i < _widgets.size(); ++i)
        {
            const auto* widget = _widgets[i];
            connect(widget, &widget_type::valueChanged, this, [this, i](const auto value) {
                _value[i] = value;
                update_widgets();

                emit value_changed(_value);
            });
        }
    }

    void double_vector2::set_value(const value_type value)
    {
        _value = value;
        update_widgets();

        emit value_changed(_value);
    }

    double_vector3::double_vector3(QWidget* parent)
        : numeric_vector(parent)
    {
        for (size_t i = 0; i < _widgets.size(); ++i)
        {
            const auto* widget = _widgets[i];
            connect(widget, &widget_type::valueChanged, this, [this, i](const auto value) {
                _value[i] = value;
                update_widgets();

                emit value_changed(_value);
            });
        }
    }

    void double_vector3::set_value(const value_type value)
    {
        _value = value;
        update_widgets();

        emit value_changed(_value);
    }

    double_vector4::double_vector4(QWidget* parent)
        : numeric_vector(parent)
    {
        for (size_t i = 0; i < _widgets.size(); ++i)
        {
            const auto* widget = _widgets[i];
            connect(widget, &widget_type::valueChanged, this, [this, i](const auto value) {
                _value[i] = value;
                update_widgets();

                emit value_changed(_value);
            });
        }
    }

    void double_vector4::set_value(const value_type value)
    {
        _value = value;
        update_widgets();

        emit value_changed(_value);
    }
} // namespace cathedral::editor