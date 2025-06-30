#pragma once

#include <QCheckBox>
#include <QHBoxLayout>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <QWidget>

namespace cathedral::editor
{
    namespace internal
    {
        template <size_t Dimensions>
            requires(Dimensions >= 2 && Dimensions <= 4)
        class bool_vector : public QWidget
        {
        public:
            using value_type = std::array<bool, Dimensions>;

            explicit bool_vector(QWidget* parent = nullptr)
                : QWidget(parent)
            {
                auto* layout = new QHBoxLayout(this);
                setLayout(layout);

                for (size_t i = 0; i < Dimensions; i++)
                {
                    auto* checkbox = new QCheckBox(this);
                    layout->addWidget(checkbox, Qt::AlignRight);
                    _checkboxes[i] = checkbox;
                }
            }

        protected:
            std::array<QCheckBox*, Dimensions> _checkboxes;
            value_type _value;

            void update_checkboxes()
            {
                for (size_t i = 0; i < Dimensions; i++)
                {
                    _checkboxes[i]->setChecked(_value[i]);
                }
            }
        };
    }

    class bool_vector2 final : public internal::bool_vector<2>
    {
        Q_OBJECT

    public:
        explicit bool_vector2(QWidget* parent = nullptr);
        void set_value(value_type value);

    signals:
        void value_changed(value_type value);
    };

    class bool_vector3 final : public internal::bool_vector<3>
    {
        Q_OBJECT

    public:
        explicit bool_vector3(QWidget* parent = nullptr);
        void set_value(value_type value);

    signals:
        void value_changed(value_type value);
    };

    class bool_vector4 final : public internal::bool_vector<4>
    {
        Q_OBJECT

    public:
        explicit bool_vector4(QWidget* parent = nullptr);
        void set_value(value_type value);

    signals:
        void value_changed(value_type value);
    };

} // namespace cathedral::editor