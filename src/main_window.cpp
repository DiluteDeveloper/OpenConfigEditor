#include "main_window.h"
#include "lsp/lsp_client.h"
#include "status_bar.h"
#include "text_editor.h"

#include <QAbstractTextDocumentLayout>
#include <QFile>
#include <QFileDialog>
#include <QGridLayout>
#include <QMenu>
#include <QMenuBar>
#include <QPlainTextEdit>
#include <QString>
#include <QTextDocument>

// QT widgets are deleted by their parent widget, hence why calling
// 'new QGridLayout' and giving it a parent and not calling 'delete'
// is okay

namespace OpenConfigEditor {

constexpr std::string_view window_title = "OpenConfigEditor";

MainWindow::MainWindow(QWidget *parent) {
  qDebug() << "Initialising window";
  QWidget *centralWidget = new QWidget;

  setWindowTitle(QString::fromUtf8(window_title));

  QGridLayout *layout = new QGridLayout;

  // Stack allocated, since never parented
  // QFile file("/etc/hostname");
  // if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
  //   std::cerr << "Failed to read file /etc/hostname" << std::endl;
  //   throw std::exception();
  // }
  //
  // // Stack allocated, since never parented
  // QString fileString = QTextStream(&file).readAll();

  StatusBar *status_bar = new StatusBar();
  TextEditor *text_edit = new TextEditor(*status_bar);

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

  connect(file_menu_new_action, &QAction::triggered, this,
          [text_edit, status_bar]() { text_edit->new_file(*status_bar); });
  connect(file_menu_open_action, &QAction::triggered, this,
          [text_edit, status_bar]() { text_edit->open_file(*status_bar); });
  connect(file_menu_save_action, &QAction::triggered, this,
          [text_edit, status_bar]() { text_edit->save_file(*status_bar); });

  // setMenuBar(create_menu_bar_widget());
  layout->addWidget(text_edit, 2, 0);
  layout->addLayout(status_bar, 1, 0);
  centralWidget->setLayout(layout);

  setCentralWidget(centralWidget);
}

} // namespace OpenConfigEditor
