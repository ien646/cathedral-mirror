#include <cathedral/editor/common/uint_vector.hpp>

namespace cathedral::editor
{
    uint_vector2::uint_vector2(QWidget* parent)
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

    void uint_vector2::set_value(const value_type value)
    {
        _value = value;
        update_widgets();

        emit value_changed(_value);
    }

    uint_vector3::uint_vector3(QWidget* parent)
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

    void uint_vector3::set_value(const value_type value)
    {
        _value = value;
        update_widgets();

        emit value_changed(_value);
    }

    uint_vector4::uint_vector4(QWidget* parent)
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

    void uint_vector4::set_value(const value_type value)
    {
        _value = value;
        update_widgets();

        emit value_changed(_value);
    }
} // namespace cathedral::editor