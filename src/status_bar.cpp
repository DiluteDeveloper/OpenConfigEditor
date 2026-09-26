#include "status_bar.h"
#include <QTimer>

namespace OpenConfigEditor {

StatusBar::StatusBar() {

  right_side_label = new QLabel;
  left_side_label = new QLabel;
  addWidget(left_side_label);
  addStretch();
  addWidget(right_side_label);
}

void StatusBar::set_file_name(const QString &file_name) {
  qDebug() << "Setting status bar file name:" << file_name;
  left_side_label->setText(file_name);
}
void StatusBar::set_bytes_written(uint32_t bytes) {

  qDebug() << "Setting status bar bytes written:" << bytes;
  right_side_label->setText(QString("%1 bytes written").arg(bytes));

  QTimer::singleShot(bytes_written_display_timeout_ms, this,
                     [this]() { right_side_label->setText(""); });
}

} // namespace OpenConfigEditor
