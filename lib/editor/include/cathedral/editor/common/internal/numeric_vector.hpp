#pragma once

#include <QHBoxLayout>
#include <QSpinBox>
#include <QWidget>

namespace cathedral::editor
{
    namespace internal
    {
        template <typename TValue, size_t Dimensions>
            requires(Dimensions >= 2 && Dimensions <= 4 && (std::integral<TValue> || std::floating_point<TValue>))
        class numeric_vector : public QWidget
        {
        public:
            using value_type = std::array<TValue, Dimensions>;
            using widget_type = std::conditional_t<std::integral<TValue>, QSpinBox, QDoubleSpinBox>;

            explicit numeric_vector(QWidget* parent = nullptr)
                : QWidget(parent)
            {
                auto* layout = new QHBoxLayout;
                layout->setContentsMargins(0, 0, 0, 0);
                setLayout(layout);

                for (size_t i = 0; i < Dimensions; i++)
                {
                    auto* spinbox = new widget_type(this);
                    spinbox->setMinimum(std::numeric_limits<TValue>::lowest());
                    spinbox->setMaximum(std::numeric_limits<TValue>::max());
                    layout->addWidget(spinbox, Qt::AlignLeft);
                    _widgets[i] = spinbox;
                    if constexpr(std::is_floating_point_v<TValue>)
                    {
                        spinbox->setSingleStep(0.01);
                    }
                }
            }

            void set_minimum(TValue minimum)
            {
                for (auto& spinbox : _widgets)
                {
                    spinbox->setMinimum(minimum);
                }
            }

            void set_maximum(TValue maximum)
            {
                for (auto& spinbox : _widgets)
                {
                    spinbox->setMaximum(maximum);
                }
            }

        protected:
            std::array<widget_type*, Dimensions> _widgets;
            value_type _value;

            void update_widgets()
            {
                for (size_t i = 0; i < Dimensions; i++)
                {
                    _widgets[i]->setValue(_value[i]);
                }
            }
        };
    } // namespace internal
} // namespace cathedral::editor