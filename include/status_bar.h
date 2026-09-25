#pragma once

#include <QBoxLayout>
#include <QLabel>

namespace OpenConfigEditor {

class StatusBar : public QHBoxLayout {
public:
  StatusBar();

  void set_file_name(const QString &file_name);
  void set_bytes_written(uint32_t bytes);

  uint16_t bytes_written_display_timeout_ms = 2000;

private:
  QLabel *left_side_label;
  QLabel *right_side_label;
};
} // namespace OpenConfigEditor
