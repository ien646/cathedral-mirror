#pragma once

#include <cathedral/editor/common/internal/numeric_vector.hpp>

namespace cathedral::editor
{
    class int_vector2 final : public internal::numeric_vector<int32_t, 2>
    {
        Q_OBJECT

    public:
        explicit int_vector2(QWidget* parent = nullptr);
        void set_value(value_type value);

    signals:
        void value_changed(value_type value);
    };

    class int_vector3 final : public internal::numeric_vector<int32_t, 3>
    {
        Q_OBJECT

    public:
        explicit int_vector3(QWidget* parent = nullptr);
        void set_value(value_type value);

    signals:
        void value_changed(value_type value);
    };

    class int_vector4 final : public internal::numeric_vector<int32_t, 4>
    {
        Q_OBJECT

    public:
        explicit int_vector4(QWidget* parent = nullptr);
        void set_value(value_type value);

    signals:
        void value_changed(value_type value);
    };
} // namespace cathedral::editor