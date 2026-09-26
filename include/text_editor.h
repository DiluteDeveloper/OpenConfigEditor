#pragma once

#include "status_bar.h"
#include <QFile>
#include <QPlainTextEdit>

namespace OpenConfigEditor {

class TextEditor : public QPlainTextEdit {
public:
  TextEditor(StatusBar &status_bar);
  void open_file(StatusBar &status_bar);
  void new_file(StatusBar &status_bar);
  void save_file(StatusBar &status_bar);

private:
  QFile current_file;
  QString current_file_path;

  void try_save(StatusBar &status_bar);
};

} // namespace OpenConfigEditor
