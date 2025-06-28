#include <cathedral/editor/common/float_vector.hpp>

namespace cathedral::editor
{
    float_vector2::float_vector2(QWidget* parent)
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

    void float_vector2::set_value(const value_type value)
    {
        _value = value;
        update_widgets();

        emit value_changed(_value);
    }

    float_vector3::float_vector3(QWidget* parent)
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

    void float_vector3::set_value(const value_type value)
    {
        _value = value;
        update_widgets();

        emit value_changed(_value);
    }

    float_vector4::float_vector4(QWidget* parent)
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

    void float_vector4::set_value(const value_type value)
    {
        _value = value;
        update_widgets();

        emit value_changed(_value);
    }
} // namespace cathedral::editor