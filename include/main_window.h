#pragma once

#include <QMainWindow>

namespace OpenConfigEditor {

class TextEditor;

class MainWindow : public QMainWindow {
  public:
    MainWindow(QWidget *parent = nullptr);

  private:
    // The editor that last had focus. Clicking the project tree takes focus
    // away from both editors, so hasFocus cannot be used to pick one from a
    // signal raised by the tree. Kept up to date by the editors' focused
    // signal instead.
    TextEditor *current_editor = nullptr;
};

} // namespace OpenConfigEditor
