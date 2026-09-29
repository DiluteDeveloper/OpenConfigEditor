#include "status_bar.h"
#include <QTimer>

namespace OpenConfigEditor {

StatusBar::StatusBar(QWidget *parent)
    : QHBoxLayout(parent), left_side_label(new QLabel),
      right_side_label(new QLabel) {

    addWidget(left_side_label);
    addStretch();
    addWidget(right_side_label);
}

void StatusBar::connect_text_editor_signals(const TextEditor &text_editor) {
    connect(&text_editor, &TextEditor::file_opened, this,
            &StatusBar::on_file_opened);
    connect(&text_editor, &TextEditor::document_saved, this,
            &StatusBar::on_document_saved);
    connect(&text_editor, &TextEditor::document_created, this,
            &StatusBar::on_document_created);
    connect(&text_editor, &TextEditor::focused, this,
            &StatusBar::on_text_editor_focused);
}

void StatusBar::on_file_opened(const QFile &file) {
#ifdef FULL_PATH_IN_STATUS_BAR
    left_side_label->setText(file.fileName());
#else
    set_file_name(file.filesystemFileName().filename().c_str());
#endif
}
void StatusBar::on_document_saved(const TextEditor &text_editor) {
    on_file_opened(*text_editor.get_file());
    set_bytes_written(text_editor.toPlainText().toUtf8().size());
}
void StatusBar::on_document_created() {
    static constexpr const char *NEW_FILE_NAME = "New File";
    left_side_label->setText(NEW_FILE_NAME);
}
void StatusBar::set_bytes_written(uint32_t bytes) {

    right_side_label->setText(QString("%1 bytes written").arg(bytes));

    static constexpr uint16_t BYTES_WRITTEN_DISPLAY_TIMEOUT_MS = 2000;
    QTimer::singleShot(BYTES_WRITTEN_DISPLAY_TIMEOUT_MS, this,
                       [this]() { right_side_label->setText(""); });
}
void StatusBar::on_text_editor_focused(const TextEditor &text_editor) {
    if (const QFile *file = text_editor.get_file(); file != nullptr) {

        on_file_opened(*text_editor.get_file());
    } else {
        on_document_created();
    }
}

} // namespace OpenConfigEditor
