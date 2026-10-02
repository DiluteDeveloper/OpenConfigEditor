#include "main_window.h"
#include "document_file_manager.h"
#include "io/json.h"
#include "project_tree.h"
#include "qmainwindow.h"
#include "status_bar.h"
#include "text_editor.h"

#include <QAbstractItemView>
#include <QAbstractTextDocumentLayout>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMenu>
#include <QMenuBar>
#include <QModelIndex>
#include <QPlainTextEdit>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QString>
#include <QTextDocument>

#include <utility>

// QT widgets are deleted by their parent widget, hence why calling
// 'new QGridLayout' and giving it a parent and not calling 'delete'
// is okay

namespace OpenConfigEditor {

// QStandardItemModel *test_project_tree_json() {
//     const QByteArray example_json = R"json([
//         {
//             "name": "system",
//             "path": "/etc/gibberish"
//         },
//         {
//             "name": "openconfig",
//             "virtual_nodes": [
//                 {
//                     "path": "/home/dilute/proton_drive/dev/openconfigeditor/"
//                 },
//                 {
//                     "name": "documents",
//                     "virtual_nodes": [
//                         {"name": "drafts"},
//                         {"name": "final"}
//                     ]
//                 }
//             ]
//         },
//         {
//             "name": "scratch",
//             "virtual_nodes": []
//         }
//     ])json";

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    static const QString window_title = QStringLiteral("OpenConfigEditor");
    QWidget *centralWidget = new QWidget;

    setWindowTitle(window_title);

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

    ProjectTree *project_tree = new ProjectTree();
    static const QString project_tree_json_path =
        QStringLiteral("config/project_tree.json");
    project_tree->load_project_tree_json_file(project_tree_json_path);

    DocumentFileManager *docfile_manager = new DocumentFileManager(this);

    status_bar->connect_text_editor_signals(*text_editor);
    status_bar->connect_text_editor_signals(*text_editor2);

    // Clicking a node in the tree moves focus off both editors, so which one
    // is current is tracked from the other direction, as each editor
    // reports gaining focus, and starts out as the first of them.
    current_editor = text_editor;
    connect(text_editor, &TextEditor::focused, this,
            [this, text_editor](const TextEditor &) {
                current_editor = text_editor;
            });
    connect(text_editor2, &TextEditor::focused, this,
            [this, text_editor2](const TextEditor &) {
                current_editor = text_editor2;
            });

    // QTextDocument *document = new QTextDocument(text_edit);
    // QAbstractTextDocumentLayout *document_layout =
    //     new QAbstractTextDocumentLayout(document);
    // document->setDocumentLayout(&QAbstractTextDocumentLayout(document));
    // text_edit->setDocument(document);

    QMenu *project_menu = menuBar()->addMenu(tr("&Project"));
    QMenu *file_menu = menuBar()->addMenu(tr("&File"));
    /*QMenu *edit_menu =*/menuBar()->addMenu(tr("&Edit"));
    /*QMenu *help_menu =*/menuBar()->addMenu(tr("&Help"));

    QAction *project_menu_reload_action = new QAction;

    QAction *file_menu_new_action = new QAction;
    QAction *file_menu_open_action = new QAction;
    QAction *file_menu_save_action = new QAction;

    project_menu_reload_action->setText("Reload");
    file_menu_new_action->setText("New");
    file_menu_open_action->setText("Open");
    file_menu_save_action->setText("Save");

    project_menu->addAction(project_menu_reload_action);

    file_menu->addAction(file_menu_new_action);
    file_menu->addAction(file_menu_open_action);
    file_menu->addAction(file_menu_save_action);

    connect(file_menu_new_action, &QAction::triggered, text_editor,
            [docfile_manager, text_editor, text_editor2]() {
                if (text_editor->hasFocus()) {

                    text_editor->new_document(*docfile_manager);
                } else if (text_editor2->hasFocus()) {

                    text_editor2->new_document(*docfile_manager);
                }
            });
    connect(file_menu_open_action, &QAction::triggered, text_editor,
            [docfile_manager, text_editor, text_editor2]() {
                if (text_editor->hasFocus()) {
                    text_editor->open_file_popup(*docfile_manager);
                } else if (text_editor2->hasFocus()) {
                    text_editor2->open_file_popup(*docfile_manager);
                }
            });
    connect(file_menu_save_action, &QAction::triggered, text_editor,
            [docfile_manager, text_editor, text_editor2]() {
                if (text_editor->hasFocus()) {
                    text_editor->save_current_document(*docfile_manager);
                } else if (text_editor2->hasFocus()) {
                    text_editor2->save_current_document(*docfile_manager);
                }
            });
    connect(project_menu_reload_action, &QAction::triggered, text_editor,
            [project_tree]() {
                project_tree->load_project_tree_json_file(
                    project_tree_json_path);
            });

    // QTreeView has no clicked signal of its own, the closest thing is
    // QAbstractItemView::pressed, which carries the index of the node the
    // mouse went down on. Only a node holding a filepath under
    // FILE_PATH_ROLE opens anything, so directories and virtual nodes, which
    // have nothing to open, leave the editor alone.
    connect(project_tree, &QAbstractItemView::pressed, project_tree,
            [this, project_tree, docfile_manager](const QModelIndex &index) {
                std::optional<QFileInfo> file = project_tree->get_file(index);
                if (!file.has_value())
                    return;

                if (current_editor != nullptr)
                    current_editor->open_file(*docfile_manager,
                                              file.value().filePath());
            });

    // setMenuBar(create_menu_bar_widget());
    layout->addWidget(text_editor, 2, 1);
    layout->addWidget(text_editor2, 3, 1);
    layout->addWidget(project_tree, 0, 0, -1, 1);
    layout->addLayout(status_bar, 1, 1);
    centralWidget->setLayout(layout);

    setCentralWidget(centralWidget);
}

} // namespace OpenConfigEditor
