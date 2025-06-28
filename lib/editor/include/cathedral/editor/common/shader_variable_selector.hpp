#pragma once

#include <cathedral/gfx/shader_data_types.hpp>

#include <QWidget>

namespace cathedral::editor
{
    class shader_variable_selector final : public QWidget
    {
        Q_OBJECT

    public:
        explicit shader_variable_selector(const std::string& name, gfx::shader_data_type type, QWidget* parent = nullptr);

        void set_value(const gfx::shader_data_value& value) const;

    signals:
        void value_changed(const gfx::shader_data_value& value);

    private:
        QWidget* _widget = nullptr;
    };
} // namespace cathedral::editor