#include <QApplication>

#include "main_window.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  ConfigEditor::MainWindow window;
  window.show();
  return app.exec();
}
