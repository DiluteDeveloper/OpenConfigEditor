#pragma once

#include "text_editor.h"
#include <QBoxLayout>
#include <QLabel>

namespace OpenConfigEditor {

class StatusBar : public QHBoxLayout {
  public:
    StatusBar(QWidget *parent = nullptr);

    void connect_text_editor_signals(const TextEditor &text_editor);

    void on_file_opened(const TextEditor &text_editor);
    void on_file_saved(const TextEditor &text_editor);
    void on_file_created();

  private:
    QLabel *left_side_label;
    QLabel *right_side_label;

    void set_bytes_written(uint32_t bytes);
};
} // namespace OpenConfigEditor
