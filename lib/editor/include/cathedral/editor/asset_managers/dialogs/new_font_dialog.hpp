#pragma once

#include <cathedral/core.hpp>

#include <glm/vec2.hpp>

#include <QDialog>

FORWARD_CLASS_INLINE(QLabel);
FORWARD_CLASS_INLINE(QLineEdit);

namespace cathedral::editor
{
    class new_font_dialog final : public QDialog
    {
    public:
        new_font_dialog(QWidget* parent, const QStringList& forbidden_names);

        const auto& result_name() const { return _name; }

        const auto& result_font() const { return _font_file; }

        auto result_glyph_height() const { return _glyph_height; }

        auto result_atlas_size() const { return glm::uvec2{ _atlas_width, _atlas_height }; }

        auto char_offset() const { return _char_offset; }

    private:
        QStringList _forbidden_names;

        QString _name = "";
        QString _font_file = "";
        int _glyph_height = 12;
        int _atlas_width = 1024;
        int _atlas_height = 1024;
        int _char_offset = 0;

        QPushButton* _gen_button = nullptr;
        QLineEdit* _font_edit = nullptr;
        QLabel* _char_gen_count = nullptr;

        void refresh_props() const;
    };
} // namespace cathedral::editor