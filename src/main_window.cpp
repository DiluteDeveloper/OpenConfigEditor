#include "main_window.h"
#include <QMenu>
#include <QMenuBar>

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

  QMenu *file_menu = menuBar()->addMenu(tr("&File"));
  QMenu *edit_menu = menuBar()->addMenu(tr("&Edit"));
  QMenu *help_menu = menuBar()->addMenu(tr("&Help"));

  QAction *file_menu_new_action = new QAction;
  QAction *file_menu_open_action = new QAction;
  QAction *file_menu_save_action = new QAction;

  file_menu_new_action->setText("New");
  file_menu_open_action->setText("Open");
  file_menu_save_action->setText("Save");
  file_menu->addAction(file_menu_new_action);
  file_menu->addAction(file_menu_open_action);
  file_menu->addAction(file_menu_save_action);

  // setMenuBar(create_menu_bar_widget());
  layout->addWidget(text_edit, 1, 0);
  centralWidget->setLayout(layout);

  setCentralWidget(centralWidget);
}

} // namespace OpenConfigEditor
