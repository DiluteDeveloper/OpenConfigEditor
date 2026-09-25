#include <QFileDialog>
#include <iostream>

#include "file_menu.h"

namespace OpenConfigEditor {

void FileMenu::on_new_file(TextEditor &text_editor, StatusBar &status_bar) {}
void FileMenu::on_open_file(TextEditor &text_editor, StatusBar &status_bar) {

  QString file_path =
      QFileDialog::getOpenFileName(&text_editor, "Open File", "/home/dilute");

  qDebug() << "Opening file" << file_path;

  text_editor.open_file(file_path, status_bar);
}
void FileMenu::on_save_file(TextEditor &text_editor, StatusBar &status_bar) {

  QFile &file = text_editor.get_current_file();

  qDebug() << "Saving file \"" << file.filesystemFileName().c_str() << "\"";

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    qWarning() << "Failed to open file"
               << QString(file.filesystemFileName().c_str());
    return;
  }
  QTextStream out(&file);
  out << text_editor.toPlainText();
  file.close();

  qDebug() << "Successfully saved file"
           << QString(file.filesystemFileName().c_str());

  status_bar.set_bytes_written(text_editor.toPlainText().toUtf8().size());
}

} // namespace OpenConfigEditor
