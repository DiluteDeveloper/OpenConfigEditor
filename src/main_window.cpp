#include "main_window.h"
#include "document_file_manager.h"
#include "qmainwindow.h"
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

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
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

    TextEditor *text_editor = new TextEditor();
    TextEditor *text_editor2 = new TextEditor();
    StatusBar *status_bar = new StatusBar();
    DocumentFileManager *docfile_manager = new DocumentFileManager(this);

    status_bar->connect_text_editor_signals(*text_editor);
    status_bar->connect_text_editor_signals(*text_editor2);

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

    connect(file_menu_new_action, &QAction::triggered, text_editor,
            [docfile_manager, text_editor, text_editor2]() {
                if (text_editor->hasFocus()) {

                    text_editor->on_user_request_create_new_document(
                        *docfile_manager);
                } else if (text_editor2->hasFocus()) {

                    text_editor2->on_user_request_create_new_document(
                        *docfile_manager);
                }
            });
    connect(file_menu_open_action, &QAction::triggered, text_editor,
            [docfile_manager, text_editor, text_editor2]() {
                if (text_editor->hasFocus()) {
                    text_editor->on_user_request_open_file(*docfile_manager);
                } else if (text_editor2->hasFocus()) {
                    text_editor2->on_user_request_open_file(*docfile_manager);
                }
            });
    connect(file_menu_save_action, &QAction::triggered, text_editor,
            [docfile_manager, text_editor, text_editor2]() {
                if (text_editor->hasFocus()) {
                    text_editor->on_user_request_save_current_document(
                        *docfile_manager);
                } else if (text_editor2->hasFocus()) {
                    text_editor2->on_user_request_save_current_document(
                        *docfile_manager);
                }
            });

    // setMenuBar(create_menu_bar_widget());
    layout->addWidget(text_editor, 2, 0);
    layout->addWidget(text_editor2, 3, 0);
    layout->addLayout(status_bar, 1, 0);
    centralWidget->setLayout(layout);

    setCentralWidget(centralWidget);
}

} // namespace OpenConfigEditor
