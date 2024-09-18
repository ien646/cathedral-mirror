#include <cathedral/editor/common/texture_slot_widget.hpp>

#include <cathedral/project/assets/texture_asset.hpp>

#include "ui_texture_slot_widget.h"

namespace cathedral::editor
{
    const QString base_stylesheet =
        "QWidget#texture_slot_widget{ border-style: solid; border-width: 1px; border-color: rgba(0, 0, 0, 20); }";

    texture_slot_widget::texture_slot_widget(QWidget* parent)
        : QFrame(parent)
        , _ui(new Ui::texture_slot_widget)
    {
        _ui->setupUi(this);
        setObjectName("texture_slot_widget");
        setStyleSheet(base_stylesheet);
        setMouseTracking(true);
    }

    void texture_slot_widget::markSelected()
    {
        _selected = true;
        setStyleSheet("QWidget#texture_slot_widget{ background-color: rgba(100, 100, 200, 40); border-style: solid; "
                      "border-width: 1px; border-color: yellow; }");
    }

    void texture_slot_widget::unmarkSelected()
    {
        _selected = false;
        setStyleSheet(base_stylesheet);
    }

    void texture_slot_widget::set_slot_index(uint32_t index)
    {
        _ui->label_Slot->setText(QString::number(index));
    }

    void texture_slot_widget::set_name(const QString& name)
    {
        _ui->label_Name->setText(name);
    }

    void texture_slot_widget::set_dimensions(uint32_t width, uint32_t height)
    {
        _ui->label_Dim->setText(QString::fromStdString(std::format("{}x{}", width, height)));
    }

    void texture_slot_widget::set_format(const QString& format)
    {
        _ui->label_Format->setText(format);
    }

    void texture_slot_widget::set_image(QImage img)
    {
        _image = img;
        _ui->label_Image->setPixmap(QPixmap::fromImage(*_image).scaled(
            _ui->label_Image->width(),
            _ui->label_Image->height(),
            Qt::AspectRatioMode::KeepAspectRatio,
            Qt::TransformationMode::SmoothTransformation));
    }

    void texture_slot_widget::mousePressEvent([[maybe_unused]] QMouseEvent* ev)
    {
        emit clicked();
    }

    void texture_slot_widget::enterEvent([[maybe_unused]] QEnterEvent* ev)
    {
        if (!_selected)
        {
            setStyleSheet("QWidget#texture_slot_widget{background-color: rgba(100, 100, 200, 25); border-style: solid; "
                          "border-width: 1px; border-color: black; }");
        }
    }

    void texture_slot_widget::leaveEvent([[maybe_unused]] QEvent* ev)
    {
        if (!_selected)
        {
            setStyleSheet(base_stylesheet);
        }
    }

    void texture_slot_widget::resizeEvent([[maybe_unused]] QResizeEvent* ev)
    {
        if (_image) // might not be loaded yet
        {
            _ui->label_Image->setPixmap(QPixmap::fromImage(*_image).scaled(
                _ui->label_Image->width(),
                _ui->label_Image->height(),
                Qt::AspectRatioMode::KeepAspectRatio,
                Qt::TransformationMode::SmoothTransformation));
        }
    }
} // namespace cathedral::editor