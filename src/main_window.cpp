#include "main_window.h"

#include <QAbstractTextDocumentLayout>
#include <QFile>
#include <QGridLayout>
#include <QPlainTextEdit>
#include <QString>
#include <QTextDocument>
#include <iostream>

// QT widgets are deleted by their parent widget, hence why calling
// 'new QGridLayout' and giving it a parent and not calling 'delete'
// is okay

namespace OpenConfigEditor {

constexpr std::string_view window_title = "OpenConfigEditor";

MainWindow::MainWindow(QWidget *parent) {
  QWidget *centralWidget = new QWidget;

  setWindowTitle(QString::fromUtf8(window_title));

  QGridLayout *layout = new QGridLayout;

  // Stack allocated, since never parented
  QFile file("/etc/hostname");
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    std::cerr << "Failed to read file /etc/hostname" << std::endl;
    throw std::exception();
  }

  std::cout << "Hello World! 0" << std::endl;
  // Stack allocated, since never parented
  QString fileString = QTextStream(&file).readAll();

  QPlainTextEdit *text_edit = new QPlainTextEdit(fileString, centralWidget);

  // QTextDocument *document = new QTextDocument(text_edit);
  // QAbstractTextDocumentLayout *document_layout =
  //     new QAbstractTextDocumentLayout(document);
  // document->setDocumentLayout(&QAbstractTextDocumentLayout(document));
  // text_edit->setDocument(document);

  layout->addWidget(text_edit, 0, 0);
  centralWidget->setLayout(layout);

  setCentralWidget(centralWidget);
}

} // namespace OpenConfigEditor
