#include <QFile>
#include <iostream>

#include "text_editor.h"

namespace OpenConfigEditor {

void TextEditor::open_file(QString *file_path) {

  QFile file(*file_path);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    std::cerr << "Failed to read file: " << file_path << std::endl;
    throw std::exception();
  }

  setPlainText(QTextStream(&file).readAll());
}

} // namespace OpenConfigEditor
