#pragma once

#include <QFrame>

namespace Ui
{
    class texture_slot_widget;
}

namespace cathedral::project
{
    class texture_asset;
}

namespace cathedral::editor
{
    class texture_slot_widget : public QFrame
    {
        Q_OBJECT

    public:
        texture_slot_widget(QWidget* parent);

        void markSelected();
        void unmarkSelected();

        void set_slot_index(uint32_t index);
        void set_name(const QString& name);
        void set_dimensions(uint32_t width, uint32_t height);
        void set_format(const QString& format);
        void set_image(const QImage& img);

    signals:
        void clicked();

    private:
        Ui::texture_slot_widget* _ui = nullptr;
        std::optional<QImage> _image;
        bool _selected = false;

        void mousePressEvent(QMouseEvent* ev) override;
        void enterEvent(QEnterEvent* ev) override;
        void leaveEvent(QEvent* ev) override;
        void resizeEvent(QResizeEvent* ev) override;
    };
} // namespace cathedral::editor