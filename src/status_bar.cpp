#include "status_bar.h"

namespace OpenConfigEditor {

StatusBar::StatusBar() {

  label = new QLabel;
  addWidget(label);
}

void StatusBar::set_file_name(const QString &file_name) {
  label->setText(file_name);
}

} // namespace OpenConfigEditor
