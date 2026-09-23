#include <QFileDialog>

#include "file_actions.h"

namespace OpenConfigEditor {

void on_file_open(TextEditor *text_editor) {
  QString file_path =
      QFileDialog::getOpenFileName(text_editor, "Open File", "/home/dilute");

  text_editor->open_file(&file_path);
}

} // namespace OpenConfigEditor
