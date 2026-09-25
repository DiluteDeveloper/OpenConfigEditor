#include <QFile>

#include "text_editor.h"
#include <QtLogging>

namespace OpenConfigEditor {

void TextEditor::open_file(QString &file_path, StatusBar &status_bar) {

  QFile file(file_path);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qWarning() << "Failed to read file: " << file_path;
    throw std::exception();
  }
  setPlainText(QTextStream(&file).readAll());
#ifdef FULL_PATH_IN_STATUS_BAR
  status_bar.set_file_name(file_path);
#else
  status_bar.set_file_name(file.filesystemFileName().filename().c_str());
#endif
}

} // namespace OpenConfigEditor
