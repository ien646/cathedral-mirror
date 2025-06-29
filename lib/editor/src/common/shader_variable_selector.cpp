#include "cathedral/editor/common/bool_vector.hpp"
#include "cathedral/editor/common/double_vector.hpp"
#include "cathedral/editor/common/float_grid.hpp"
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
    namespace
    {
        template <size_t Cols, size_t Rows>
        glm::mat<Cols, Rows, float> vec_to_mat(const std::vector<float>& v)
        {
            glm::mat<Cols, Rows, float> result;
            for (size_t i = 0; i < Cols; ++i)
            {
                for (size_t j = 0; j < Rows; ++j)
                {
                    result[i][j] = v[(i * Rows) + j];
                }
            }
            return result;
        }

        template <typename... Ts> // (7)
        struct overload : Ts...
        {
            using Ts::operator()...;
        };
        template <class... Ts>
        overload(Ts...) -> overload<Ts...>;

        template <typename T, size_t Dims>
        std::array<T, Dims> glmvec_to_array(const glm::vec<Dims, T>& vec)
        {
            std::array<T, Dims> result;
            for (size_t i = 0; i < Dims; ++i)
            {
                result[i] = vec[i];
            }
            return result;
        }

        template <size_t Cols, size_t Rows>
        std::vector<float> glmmat_to_vector(const glm::mat<Cols, Rows, float>& mat)
        {
            std::vector<float> result;
            for (size_t i = 0; i < Cols; ++i)
            {
                for (size_t j = 0; j < Rows; ++j)
                {
                    result.push_back(mat[i][j]);
                }
            }
            return result;
        }
    } // namespace

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
            widget->setMinimum(0);
            widget->setMaximum(std::numeric_limits<int32_t>::max());
            layout->addWidget(widget, 1);
            connect(widget, &QSpinBox::valueChanged, this, [this, type](const int value) {
                if (type == gfx::shader_data_type::INT)
                {
                    emit value_changed(value);
                }
                else
                {
                    emit value_changed(static_cast<uint32_t>(value));
                }
            });
            _widget = widget;
            break;
        }
            // FLOATING POINT
        case gfx::shader_data_type::FLOAT:
        case gfx::shader_data_type::DOUBLE: {
            auto* widget = new QDoubleSpinBox(this);
            widget->setSingleStep(0.01);
            widget->setMinimum(
                type == gfx::shader_data_type::FLOAT ? std::numeric_limits<float>::lowest()
                                                     : std::numeric_limits<double>::lowest());
            widget->setMaximum(
                type == gfx::shader_data_type::FLOAT ? std::numeric_limits<float>::max()
                                                     : std::numeric_limits<double>::max());
            layout->addWidget(widget, 1);
            connect(widget, &QDoubleSpinBox::valueChanged, this, [this, type](const double value) {
                if (type == gfx::shader_data_type::FLOAT)
                {
                    emit value_changed(static_cast<float>(value));
                }
                else
                {
                    emit value_changed(value);
                }
            });
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
        case gfx::shader_data_type::MAT2X2: {
            auto* widget = new float_grid({ 2, 2 }, this);
            layout->addWidget(widget, 1);
            connect(widget, &float_grid::value_changed, this, [this](const auto& value) {
                emit value_changed(vec_to_mat<2, 2>(value));
            });
            _widget = widget;
            break;
        }
        case gfx::shader_data_type::MAT2X3: {
            auto* widget = new float_grid({ 2, 3 }, this);
            layout->addWidget(widget, 1);
            connect(widget, &float_grid::value_changed, this, [this](const auto& value) {
                emit value_changed(vec_to_mat<2, 3>(value));
            });
            _widget = widget;
            break;
        }
        case gfx::shader_data_type::MAT2X4: {
            auto* widget = new float_grid({ 2, 4 }, this);
            layout->addWidget(widget, 1);
            connect(widget, &float_grid::value_changed, this, [this](const auto& value) {
                emit value_changed(vec_to_mat<2, 4>(value));
            });
            _widget = widget;
            break;
        }
        case gfx::shader_data_type::MAT3X2: {
            auto* widget = new float_grid({ 3, 2 }, this);
            layout->addWidget(widget, 1);
            connect(widget, &float_grid::value_changed, this, [this](const auto& value) {
                emit value_changed(vec_to_mat<3, 2>(value));
            });
            _widget = widget;
            break;
        }
        case gfx::shader_data_type::MAT3X3: {
            auto* widget = new float_grid({ 3, 3 }, this);
            layout->addWidget(widget, 1);
            connect(widget, &float_grid::value_changed, this, [this](const auto& value) {
                emit value_changed(vec_to_mat<3, 3>(value));
            });
            _widget = widget;
            break;
        }
        case gfx::shader_data_type::MAT3X4: {
            auto* widget = new float_grid({ 3, 4 }, this);
            layout->addWidget(widget, 1);
            connect(widget, &float_grid::value_changed, this, [this](const auto& value) {
                emit value_changed(vec_to_mat<3, 4>(value));
            });
            _widget = widget;
            break;
        }
        case gfx::shader_data_type::MAT4X2: {
            auto* widget = new float_grid({ 4, 2 }, this);
            layout->addWidget(widget, 1);
            connect(widget, &float_grid::value_changed, this, [this](const auto& value) {
                emit value_changed(vec_to_mat<4, 2>(value));
            });
            _widget = widget;
            break;
        }
        case gfx::shader_data_type::MAT4X3: {
            auto* widget = new float_grid({ 4, 3 }, this);
            layout->addWidget(widget, 1);
            connect(widget, &float_grid::value_changed, this, [this](const auto& value) {
                emit value_changed(vec_to_mat<4, 3>(value));
            });
            _widget = widget;
            break;
        }
        case gfx::shader_data_type::MAT4X4: {
            auto* widget = new float_grid({ 4, 4 }, this);
            layout->addWidget(widget, 1);
            connect(widget, &float_grid::value_changed, this, [this](const auto& value) {
                emit value_changed(vec_to_mat<4, 4>(value));
            });
            _widget = widget;
            break;
        }
        default:
            CRITICAL_ERROR("Unhandled shader data type");
        }
    }

    void shader_variable_selector::set_value(const gfx::shader_data_value& value) const
    {
        // clang-format off
        auto set_value_overload = overload{
            [this](const bool& val) { dynamic_cast<QCheckBox*>(_widget)->setChecked(val); },
            [this](const int32_t& val) { dynamic_cast<QSpinBox*>(_widget)->setValue(val); },
            [this](const uint32_t& val) { dynamic_cast<QSpinBox*>(_widget)->setValue(static_cast<int>(val)); },
            [this](const float& val) { dynamic_cast<QDoubleSpinBox*>(_widget)->setValue(val); },
            [this](const double& val) { dynamic_cast<QDoubleSpinBox*>(_widget)->setValue(val); },
            [this](const glm::bvec2& val) { dynamic_cast<bool_vector2*>(_widget)->set_value(glmvec_to_array<bool, 2>(val)); },
            [this](const glm::bvec3& val) { dynamic_cast<bool_vector3*>(_widget)->set_value(glmvec_to_array<bool, 3>(val)); },
            [this](const glm::bvec4& val) { dynamic_cast<bool_vector4*>(_widget)->set_value(glmvec_to_array<bool, 4>(val)); },
            [this](const glm::ivec2& val) { dynamic_cast<int_vector2*>(_widget)->set_value(glmvec_to_array<int32_t, 2>(val)); },
            [this](const glm::ivec3& val) { dynamic_cast<int_vector3*>(_widget)->set_value(glmvec_to_array<int32_t, 3>(val)); },
            [this](const glm::ivec4& val) { dynamic_cast<int_vector4*>(_widget)->set_value(glmvec_to_array<int32_t, 4>(val)); },
            [this](const glm::uvec2& val) { dynamic_cast<uint_vector2*>(_widget)->set_value(glmvec_to_array<uint32_t, 2>(val)); },
            [this](const glm::uvec3& val) { dynamic_cast<uint_vector3*>(_widget)->set_value(glmvec_to_array<uint32_t, 3>(val)); },
            [this](const glm::uvec4& val) { dynamic_cast<uint_vector4*>(_widget)->set_value(glmvec_to_array<uint32_t, 4>(val)); },
            [this](const glm::dvec2& val) { dynamic_cast<double_vector2*>(_widget)->set_value(glmvec_to_array<double, 2>(val)); },
            [this](const glm::dvec3& val) { dynamic_cast<double_vector3*>(_widget)->set_value(glmvec_to_array<double, 3>(val)); },
            [this](const glm::dvec4& val) { dynamic_cast<double_vector4*>(_widget)->set_value(glmvec_to_array<double, 4>(val)); },
            [this](const glm::vec2& val) { dynamic_cast<float_vector2*>(_widget)->set_value(glmvec_to_array<float, 2>(val)); },
            [this](const glm::vec3& val) { dynamic_cast<float_vector3*>(_widget)->set_value(glmvec_to_array<float, 3>(val)); },
            [this](const glm::vec4& val) { dynamic_cast<float_vector4*>(_widget)->set_value(glmvec_to_array<float, 4>(val)); },
            [this](const glm::mat2x2& val) { dynamic_cast<float_grid*>(_widget)->set_values(glmmat_to_vector<2, 2>(val)); },
            [this](const glm::mat2x3& val) { dynamic_cast<float_grid*>(_widget)->set_values(glmmat_to_vector<2, 3>(val)); },
            [this](const glm::mat2x4& val) { dynamic_cast<float_grid*>(_widget)->set_values(glmmat_to_vector<2, 4>(val)); },
            [this](const glm::mat3x2& val) { dynamic_cast<float_grid*>(_widget)->set_values(glmmat_to_vector<3, 2>(val)); },
            [this](const glm::mat3x3& val) { dynamic_cast<float_grid*>(_widget)->set_values(glmmat_to_vector<3, 3>(val)); },
            [this](const glm::mat3x4& val) { dynamic_cast<float_grid*>(_widget)->set_values(glmmat_to_vector<3, 4>(val)); },
            [this](const glm::mat4x2& val) { dynamic_cast<float_grid*>(_widget)->set_values(glmmat_to_vector<4, 2>(val)); },
            [this](const glm::mat4x3& val) { dynamic_cast<float_grid*>(_widget)->set_values(glmmat_to_vector<4, 3>(val)); },
            [this](const glm::mat4x4& val) { dynamic_cast<float_grid*>(_widget)->set_values(glmmat_to_vector<4, 4>(val)); }
        };
        // clang-format on

        std::visit(set_value_overload, value);
    }
} // namespace cathedral::editor