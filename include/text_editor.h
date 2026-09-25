#pragma once

#include "status_bar.h"
#include <QPlainTextEdit>

namespace OpenConfigEditor {

class TextEditor : public QPlainTextEdit {
public:
  void open_file(QString &file_path, StatusBar &status_bar);
};

} // namespace OpenConfigEditor
