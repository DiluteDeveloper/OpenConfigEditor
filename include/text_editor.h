#pragma once

#include <QPlainTextEdit>
namespace OpenConfigEditor {

class TextEditor : public QPlainTextEdit {
public:
  void open_file(QString *file_path);
};

} // namespace OpenConfigEditor
