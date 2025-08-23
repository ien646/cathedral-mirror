#include <QComboBox>
#include <QFileDialog>
#include <cathedral/editor/asset_managers/dialogs/new_font_dialog.hpp>

#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <filesystem>

namespace cathedral::editor
{
    new_font_dialog::new_font_dialog(QWidget* parent, const QStringList& forbidden_names)
        : QDialog(parent)
        , _forbidden_names(forbidden_names)
    {
        setMinimumWidth(420);

        auto* main_layout = new QFormLayout;
        setLayout(main_layout);

        auto* name_edit = new QLineEdit;
        main_layout->addRow("Name:", name_edit);

        _font_edit = new QLineEdit;
        main_layout->addRow("Font:", _font_edit);

        auto* browse_button = new QPushButton("Browse...");
        main_layout->addRow(browse_button);

        const QStringList atlas_sizes = { "128", "256", "512", "1024", "2048", "4096", "8192" };

        auto* atlas_size_x = new QComboBox;
        auto* atlas_size_y = new QComboBox;

        atlas_size_x->addItems(atlas_sizes);
        atlas_size_y->addItems(atlas_sizes);

        atlas_size_x->setCurrentText("1024");
        atlas_size_y->setCurrentText("1024");
        _atlas_width = 1024;
        _atlas_height = 1024;

        main_layout->addRow("Atlas width:", atlas_size_x);
        main_layout->addRow("Atlas height:", atlas_size_y);

        auto* glyph_height = new QComboBox;
        glyph_height->addItems(QStringList{"4", "8", "16", "32", "64", "128", "256"});
        glyph_height->setCurrentText("64");
        _glyph_height = 64;

        main_layout->addRow("Glyph height (pixels):", glyph_height);

        auto* char_gen_offset_spinbox = new QSpinBox;
        char_gen_offset_spinbox->setMinimum(_char_offset);
        main_layout->addRow("Character offset:", char_gen_offset_spinbox);

        _char_gen_count = new QLabel;
        main_layout->addRow("Charset range:", _char_gen_count);

        _gen_button = new QPushButton("Generate");
        _gen_button->setEnabled(false);

        main_layout->addRow(_gen_button);

        connect(browse_button, &QPushButton::clicked, this, [this] {
            const auto filename = QFileDialog::getOpenFileName(this, "Choose a font", {}, ".ttf");
            if (!filename.isEmpty())
            {
                _font_file = filename;
                _font_edit->setText(filename);
                refresh_props();
            }
        });

        connect(name_edit, &QLineEdit::textChanged, this, [this](const QString& text) {
            _name = text;
            refresh_props();
        });

        connect(_font_edit, &QLineEdit::textChanged, this, [this](const QString& text) {
            _font_file = text;
            refresh_props();
        });

        connect(glyph_height, &QComboBox::currentTextChanged, this, [this](const QString& value) {
            _glyph_height = value.toInt();
            refresh_props();
        });

        connect(atlas_size_x, &QComboBox::currentTextChanged, this, [this](const QString& val) {
            _atlas_width = val.toInt();
            refresh_props();
        });

        connect(atlas_size_y, &QComboBox::currentTextChanged, this, [this](const QString& val) {
            _atlas_height = val.toInt();
            refresh_props();
        });

        connect(char_gen_offset_spinbox, &QSpinBox::valueChanged, this, [this](const int val) {
            _char_offset = val;
            refresh_props();
        });

        connect(_gen_button, &QPushButton::clicked, this, [this] { accept(); });
    }

    void new_font_dialog::refresh_props() const
    {
        _gen_button->setDisabled(
            _name.isEmpty() || _font_file.isEmpty() || !std::filesystem::exists(_font_file.toStdString()) ||
            _forbidden_names.contains(_name));

        const auto cols = _atlas_width / _glyph_height;
        const auto rows = _atlas_height / _glyph_height;

        _char_gen_count->setText(QString{ "[%1 - %2]" }.arg(_char_offset).arg(cols * rows));
    }
} // namespace cathedral::editor