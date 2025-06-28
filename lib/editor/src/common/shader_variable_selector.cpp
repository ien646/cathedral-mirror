#include "cathedral/editor/common/bool_vector.hpp"
#include "cathedral/editor/common/double_vector.hpp"
#include "cathedral/editor/common/float_vector.hpp"
#include "cathedral/editor/common/int_vector.hpp"
#include "cathedral/editor/common/uint_vector.hpp"

#include <QCheckBox>
#include <cathedral/editor/common/shader_variable_selector.hpp>

#include <cathedral/editor/utils.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>

namespace cathedral::editor
{
    shader_variable_selector::shader_variable_selector(
        const std::string& name,
        const gfx::shader_data_type type,
        QWidget* parent)
        : QWidget(parent)
    {
        auto* layout = new QHBoxLayout(this);
        setLayout(layout);

        layout->addWidget(new QLabel(QSTR(name)), 0);

        switch (type)
        {
            // BOOLEAN
        case gfx::shader_data_type::BOOL: {
            auto* widget = new QCheckBox(this);
            layout->addWidget(widget, 1);
            connect(widget, &QCheckBox::toggled, this, [this](const bool toggled) { emit value_changed(toggled); });
            _widget = widget;
            break;
        }
            // INTEGRAL
        case gfx::shader_data_type::INT:
        case gfx::shader_data_type::UINT: {
            auto* widget = new QSpinBox(this);
            widget->setMinimum(
                type == gfx::shader_data_type::INT ? std::numeric_limits<int32_t>::lowest()
                                                   : std::numeric_limits<uint32_t>::lowest());
            widget->setMaximum(
                type == gfx::shader_data_type::INT ? std::numeric_limits<int32_t>::max()
                                                   : std::numeric_limits<uint32_t>::max());
            layout->addWidget(widget, 1);
            connect(widget, &QSpinBox::valueChanged, this, [this](const int value) { emit value_changed(value); });
            _widget = widget;
            break;
        }
            // FLOATING POINT
        case gfx::shader_data_type::FLOAT:
        case gfx::shader_data_type::DOUBLE: {
            auto* widget = new QDoubleSpinBox(this);
            widget->setMinimum(
                type == gfx::shader_data_type::FLOAT ? std::numeric_limits<float>::lowest()
                                                     : std::numeric_limits<double>::lowest());
            widget->setMaximum(
                type == gfx::shader_data_type::FLOAT ? std::numeric_limits<float>::max()
                                                     : std::numeric_limits<double>::max());
            layout->addWidget(widget, 1);
            connect(widget, &QDoubleSpinBox::valueChanged, this, [this](const double value) { emit value_changed(value); });
            _widget = widget;
            break;
        }
        case gfx::shader_data_type::BVEC2: {
            auto* widget = new bool_vector2(this);
            layout->addWidget(widget, 1);
            connect(widget, &bool_vector2::value_changed, this, [this](const auto value) {
                emit value_changed(glm::bvec2{ value[0], value[1] });
            });
            _widget = widget;
            break;
        }
        case gfx::shader_data_type::BVEC3: {
            auto* widget = new bool_vector3(this);
            layout->addWidget(widget, 1);
            connect(widget, &bool_vector3::value_changed, this, [this](const auto value) {
                emit value_changed(glm::bvec3{ value[0], value[1], value[2] });
            });
            _widget = widget;
            break;
        }
        case gfx::shader_data_type::BVEC4: {
            auto* widget = new bool_vector4(this);
            layout->addWidget(widget, 1);
            connect(widget, &bool_vector4::value_changed, this, [this](const auto value) {
                emit value_changed(glm::bvec4{ value[0], value[1], value[2], value[3] });
            });
            _widget = widget;
            break;
        }
        case gfx::shader_data_type::IVEC2: {
            auto* widget = new int_vector2(this);
            layout->addWidget(widget, 1);
            connect(widget, &int_vector2::value_changed, this, [this](const auto value) {
                emit value_changed(glm::ivec2{ value[0], value[1] });
            });
            _widget = widget;
            break;
        }
        case gfx::shader_data_type::IVEC3: {
            auto* widget = new int_vector3(this);
            layout->addWidget(widget, 1);
            connect(widget, &int_vector3::value_changed, this, [this](const auto value) {
                emit value_changed(glm::ivec3{ value[0], value[1], value[2] });
            });
            _widget = widget;
            break;
        }
        case gfx::shader_data_type::IVEC4: {
            auto* widget = new int_vector4(this);
            layout->addWidget(widget, 1);
            connect(widget, &int_vector4::value_changed, this, [this](const auto value) {
                emit value_changed(glm::ivec4{ value[0], value[1], value[2], value[3] });
            });
            _widget = widget;
            break;
        }
        case gfx::shader_data_type::VEC2: {
            auto* widget = new float_vector2(this);
            layout->addWidget(widget, 1);
            connect(widget, &float_vector2::value_changed, this, [this](const auto value) {
                emit value_changed(glm::vec2{ value[0], value[1] });
            });
            _widget = widget;
            break;
        }
        case gfx::shader_data_type::VEC3: {
            auto* widget = new float_vector3(this);
            layout->addWidget(widget, 1);
            connect(widget, &float_vector3::value_changed, this, [this](const auto value) {
                emit value_changed(glm::vec3{ value[0], value[1], value[2] });
            });
            _widget = widget;
            break;
        }
        case gfx::shader_data_type::VEC4: {
            auto* widget = new float_vector4(this);
            layout->addWidget(widget, 1);
            connect(widget, &float_vector4::value_changed, this, [this](const auto value) {
                emit value_changed(glm::vec4{ value[0], value[1], value[2], value[3] });
            });
            _widget = widget;
            break;
        }
        case gfx::shader_data_type::UVEC2: {
            auto* widget = new uint_vector2(this);
            layout->addWidget(widget, 1);
            connect(widget, &uint_vector2::value_changed, this, [this](const auto value) {
                emit value_changed(glm::uvec2{ value[0], value[1] });
            });
            _widget = widget;
            break;
        }
        case gfx::shader_data_type::UVEC3: {
            auto* widget = new uint_vector3(this);
            layout->addWidget(widget, 1);
            connect(widget, &uint_vector3::value_changed, this, [this](const auto value) {
                emit value_changed(glm::uvec3{ value[0], value[1], value[2] });
            });
            _widget = widget;
            break;
        }
        case gfx::shader_data_type::UVEC4: {
            auto* widget = new uint_vector4(this);
            layout->addWidget(widget, 1);
            connect(widget, &uint_vector4::value_changed, this, [this](const auto value) {
                emit value_changed(glm::uvec4{ value[0], value[1], value[2], value[3] });
            });
            _widget = widget;
            break;
        }
        case gfx::shader_data_type::DVEC2: {
            auto* widget = new double_vector2(this);
            layout->addWidget(widget, 1);
            connect(widget, &double_vector2::value_changed, this, [this](const auto value) {
                emit value_changed(glm::dvec2{ value[0], value[1] });
            });
            _widget = widget;
            break;
        }
        case gfx::shader_data_type::DVEC3: {
            auto* widget = new double_vector3(this);
            layout->addWidget(widget, 1);
            connect(widget, &double_vector3::value_changed, this, [this](const auto value) {
                emit value_changed(glm::dvec3{ value[0], value[1], value[2] });
            });
            _widget = widget;
            break;
        }
        case gfx::shader_data_type::DVEC4: {
            auto* widget = new double_vector4(this);
            layout->addWidget(widget, 1);
            connect(widget, &double_vector4::value_changed, this, [this](const auto value) {
                emit value_changed(glm::dvec4{ value[0], value[1], value[2], value[3] });
            });
            _widget = widget;
            break;
        }
        case gfx::shader_data_type::MAT2X2:
        case gfx::shader_data_type::MAT2X3:
        case gfx::shader_data_type::MAT2X4:
        case gfx::shader_data_type::MAT3X2:
        case gfx::shader_data_type::MAT3X3:
        case gfx::shader_data_type::MAT3X4:
        case gfx::shader_data_type::MAT4X2:
        case gfx::shader_data_type::MAT4X3:
        case gfx::shader_data_type::MAT4X4:
            NOT_IMPLEMENTED();
            break;
        default:
            CRITICAL_ERROR("Unhandled shader data type");
        }
    }

    void shader_variable_selector::set_value(const gfx::shader_data_value& value)
    {
    }
} // namespace cathedral::editor