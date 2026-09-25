#pragma once

#include "status_bar.h"
#include "text_editor.h"

namespace OpenConfigEditor {

class FileMenu {
public:
  static void on_new_file(TextEditor &text_editor, StatusBar &status_bar);
  static void on_open_file(TextEditor &text_editor, StatusBar &status_bar);
  static void on_save_file(TextEditor &text_editor, StatusBar &status_bar);
};

} // namespace OpenConfigEditor
