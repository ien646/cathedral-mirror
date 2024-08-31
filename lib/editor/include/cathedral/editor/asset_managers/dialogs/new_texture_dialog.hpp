#pragma once

#include <QDialog>

namespace cathedral::editor
{
    class new_texture_dialog : public QDialog
    {
    public:
        new_texture_dialog(QWidget* parent = nullptr);

        const auto& name() const { return _name; }
        const auto& image_path() const { return _path; }
        const auto& format() const { return _format; }
        auto mips() const { return _mips; }
        const auto& mipgen_filter() const { return _mipfilter; }

    private:
        QString _name{};
        QString _path{};
        QString _format{};
        int _mips{};
        QString _mipfilter{};
    };
} // namespace cathedral::editor