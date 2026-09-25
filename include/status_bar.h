#pragma once

#include <QBoxLayout>
#include <QLabel>

namespace OpenConfigEditor {

class StatusBar : public QHBoxLayout {
public:
  StatusBar();

  void set_file_name(const QString &file_name);

private:
  QLabel *label;
};
} // namespace OpenConfigEditor
