#pragma once

#include "status_bar.h"
#include <QFile>
#include <QPlainTextEdit>

namespace OpenConfigEditor {

class TextEditor : public QPlainTextEdit {
public:
  void open_file(QString &file_path, StatusBar &status_bar);

  // If no file is open, returns nullptr
  QFile &get_current_file();

private:
  QFile current_file;
};

} // namespace OpenConfigEditor
