#pragma once

#include <cathedral/engine/node_type.hpp>

#include <QDialog>

namespace cathedral::editor
{
    class add_node_dialog : public QDialog
    {
    public:
        add_node_dialog(QWidget* parent);

        struct result
        {
            std::string name;
            engine::node_type type = engine::node_type::NODE;
        };

        result result_value() const { return _result; }

    private:
        result _result;
    };
} // namespace cathedral::editor