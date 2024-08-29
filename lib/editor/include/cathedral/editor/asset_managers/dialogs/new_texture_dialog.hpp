#pragma once

#include <QDialog>

class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;

namespace ien
{
    struct image_info;
}

namespace cathedral::editor
{
    class new_texture_dialog : public QDialog
    {
    public:
        new_texture_dialog(QStringList banned_names, QWidget* parent = nullptr);

        const auto& name() const { return _name; }
        const auto& image_path() const { return _path; }
        const auto& format() const { return _format; }
        auto mips() const { return _mips; }
        const auto& mipgen_filter() const { return _mipfilter; }

    private:
        QStringList _banned_names;

        QString _name{};
        QString _path{};
        QString _format{};
        int _mips{};
        QString _mipfilter{};

        QLineEdit* _name_edit = nullptr;
        QLineEdit* _path_edit = nullptr;
        QSpinBox* _mips_spinbox = nullptr;
        QComboBox* _format_combo = nullptr;
        QComboBox* _filter_combo = nullptr;
        QPushButton* _create_button = nullptr;
        QLabel* _format_warning_label = nullptr;

        void clamp_mips(const ien::image_info& iinfo);
        void update_states();

    private slots:
        void slot_browse_clicked();
        void slot_create_clicked();
        void slot_format_changed();
    };
} // namespace cathedral::editor