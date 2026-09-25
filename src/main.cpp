#include <QApplication>
#include <QDebug>

#include "main_window.h"
#include "qlogging.h"

int main(int argc, char *argv[]) {
  qSetMessagePattern("[%{file}:%{line}] %{function}: %{message}");
  QApplication app(argc, argv);
  OpenConfigEditor::MainWindow window;
  window.show();
  return app.exec();
}
