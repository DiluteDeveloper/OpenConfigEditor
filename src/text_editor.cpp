#include <QDebug>
#include <QFile>
#include <QtLogging>

#include "text_editor.h"

namespace OpenConfigEditor {

void TextEditor::open_file(QString &file_path, StatusBar &status_bar) {

  qDebug() << "Opening file" << file_path;

  current_file.setFileName(file_path);
  if (!current_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qWarning() << "Failed to open file" << file_path;
    return;
  }
  setPlainText(QTextStream(&current_file).readAll());
  current_file.close();

#ifdef FULL_PATH_IN_STATUS_BAR
  status_bar.set_file_name(file_path);
#else
  status_bar.set_file_name(
      current_file.filesystemFileName().filename().c_str());
#endif

  qDebug() << "Successfully opened file" << file_path;
}

QFile &TextEditor::get_current_file() { return current_file; }

} // namespace OpenConfigEditor
