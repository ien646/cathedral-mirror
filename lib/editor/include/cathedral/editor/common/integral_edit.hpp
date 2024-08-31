#pragma once

#include <ien/lang_utils.hpp>

#include <QIntValidator>
#include <QLineEdit>

namespace cathedral::editor
{
    namespace detail
    {
        template <ien::concepts::Integral T>
        class integral_edit : public QLineEdit
        {
        public:
            integral_edit(
                QWidget* parent,
                T min_value = std::numeric_limits<T>::lowest(),
                T max_value = std::numeric_limits<T>::max())
                : QLineEdit(parent)
            {
                QIntValidator validator(min_value, max_value);
                setValidator(validator);
            }

            T get_value() const { return _value; }
            void set_value(T value) { _value = value; }

        protected:
            T _value;
        };
    } // namespace detail

    class int_edit : public detail::integral_edit<int32_t>
    {
        Q_SLOT
    public:
        using base_t = detail::integral_edit<int32_t>;
        using base_t::base_t;
    };

    class uint_edit : public detail::integral_edit<uint32_t>
    {
        Q_SLOT
    public:
        using base_t = detail::integral_edit<uint32_t>;
        using base_t::base_t;
    };
} // namespace cathedral::editor