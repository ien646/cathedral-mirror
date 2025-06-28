#pragma once

#include <cathedral/editor/common/internal/numeric_vector.hpp>

namespace cathedral::editor
{
    class double_vector2 final : public internal::numeric_vector<double, 2>
    {
        Q_OBJECT

    public:
        explicit double_vector2(QWidget* parent = nullptr);
        void set_value(value_type value);

        signals:
            void value_changed(value_type value);
    };

    class double_vector3 final : public internal::numeric_vector<double, 3>
    {
        Q_OBJECT

    public:
        explicit double_vector3(QWidget* parent = nullptr);
        void set_value(value_type value);

        signals:
            void value_changed(value_type value);
    };

    class double_vector4 final : public internal::numeric_vector<double, 4>
    {
        Q_OBJECT

    public:
        explicit double_vector4(QWidget* parent = nullptr);
        void set_value(value_type value);

        signals:
            void value_changed(value_type value);
    };
} // namespace cathedral::editor