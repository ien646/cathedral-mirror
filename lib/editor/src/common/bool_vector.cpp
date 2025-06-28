#include <cathedral/editor/common/bool_vector.hpp>

namespace cathedral::editor
{
    bool_vector2::bool_vector2(QWidget* parent)
        : bool_vector(parent)
    {
        for (size_t i = 0; i < _checkboxes.size(); ++i)
        {
            const auto* checkbox = _checkboxes[i];
            connect(checkbox, &QCheckBox::toggled, this, [this, i](const bool value) {
                _value[i] = value;
                update_checkboxes();

                emit value_changed(_value);
            });
        }
    }

    void bool_vector2::set_value(const value_type value)
    {
        _value = value;
        update_checkboxes();

        emit value_changed(_value);
    }

    bool_vector3::bool_vector3(QWidget* parent)
        : bool_vector(parent)
    {
        for (size_t i = 0; i < _checkboxes.size(); ++i)
        {
            const auto* checkbox = _checkboxes[i];
            connect(checkbox, &QCheckBox::toggled, this, [this, i](const bool value) {
                _value[i] = value;
                update_checkboxes();

                emit value_changed(_value);
            });
        }
    }

    void bool_vector3::set_value(const value_type value)
    {
        _value = value;
        update_checkboxes();

        emit value_changed(_value);
    }

    bool_vector4::bool_vector4(QWidget* parent)
        : bool_vector(parent)
    {
        for (size_t i = 0; i < _checkboxes.size(); ++i)
        {
            const auto* checkbox = _checkboxes[i];
            connect(checkbox, &QCheckBox::toggled, this, [this, i](const bool value) {
                _value[i] = value;
                update_checkboxes();

                emit value_changed(_value);
            });
        }
    }

    void bool_vector4::set_value(const value_type value)
    {
        _value = value;
        update_checkboxes();

        emit value_changed(_value);
    }
} // namespace cathedral::editor