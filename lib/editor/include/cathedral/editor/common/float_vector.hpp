#pragma once

#include <cathedral/editor/common/internal/numeric_vector.hpp>

namespace cathedral::editor
{
    class float_vector2 final : public internal::numeric_vector<float, 2>
    {
        Q_OBJECT

    public:
        explicit float_vector2(QWidget* parent = nullptr);
        void set_value(value_type value);

        signals:
            void value_changed(value_type value);
    };

    class float_vector3 final : public internal::numeric_vector<float, 3>
    {
        Q_OBJECT

    public:
        explicit float_vector3(QWidget* parent = nullptr);
        void set_value(value_type value);

        signals:
            void value_changed(value_type value);
    };

    class float_vector4 final : public internal::numeric_vector<float, 4>
    {
        Q_OBJECT

    public:
        explicit float_vector4(QWidget* parent = nullptr);
        void set_value(value_type value);

        signals:
            void value_changed(value_type value);
    };
} // namespace cathedral::editor