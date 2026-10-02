#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QKeyEvent>
#include <QtLogging>

#include "qjsonobject.h"
#include "qmessagebox.h"
#include "qplaintextedit.h"
#include "qtextdocument.h"
#include "text_editor.h"

namespace OpenConfigEditor {

TextEditor::TextEditor(QWidget *parent) : QTextEdit(parent) {
    lsp = new LSP::LSPClient(this);
}
void TextEditor::open_file(DocumentFileManager &docfile_manager,
                           const QString &new_file_path) {

    if (!save_current_document_popup(docfile_manager))
        return;
    if (file != nullptr) {
        docfile_manager.unload_document(file->fileName());
        file = nullptr;
    }

    DocumentFile *docfile = docfile_manager.load_or_get_document(new_file_path);

    if (docfile == nullptr) {
        qWarning() << "Failed to load file: " << new_file_path;
        return;
    }
    setDocument(&docfile->document);
    file = &docfile->file;

    emit file_opened(*file);

    // lsp->dispatch_request(LSP::LSPMessages::did_open(
    //     QUrl::fromLocalFile(current_file_path), "c", 0, toPlainText()));
}

void TextEditor::open_file_popup(DocumentFileManager &docfile_manager) {

    // Set to my home directory for testing purposes for now
    QString file_path =
        QFileDialog::getOpenFileName(this, "Open File", QDir::homePath());
    open_file(docfile_manager, file_path);
}

void TextEditor::new_document(DocumentFileManager &docfile_manager) {
    if (!save_current_document_popup(docfile_manager))
        return;

    if (file != nullptr) {
        docfile_manager.unload_document(file->fileName());
        file = nullptr;
    }

    setDocument(new QTextDocument(this));
    emit document_created();
}

void TextEditor::save_current_document(DocumentFileManager &docfile_manager) {

    if (!document()->isModified())
        return;

    // Need to address bug when user closes this QFileDialog, never
    // addressed
    if (file == nullptr) {
        QString file_path =
            QFileDialog::getSaveFileName(this, "Save File", QDir::homePath(),
                                         "Text Files (*.tt);; All Files (*)");
        DocumentFile *df =
            docfile_manager.save_document(file_path, toPlainText());
        if (df == nullptr) {
            qWarning() << "Failed to save file:" << file_path;
            return;
        }
        file = &df->file;
        setDocument(&df->document);
    }

    docfile_manager.save_document(file->fileName(), toPlainText());
    document()->setModified(false);
    emit document_saved(*this);
}

bool TextEditor::save_current_document_popup(
    DocumentFileManager &docfile_manager) {

    if (document()->isModified()) {
        switch (QMessageBox::question(this, "Your file has not been saved.",
                                      "Do you want to save your changes?",
                                      QMessageBox::Save | QMessageBox::Discard |
                                          QMessageBox::Cancel)) {
        case QMessageBox::Discard:
            if (file != nullptr)
                docfile_manager.unload_document(file->fileName());
            return true;
        case QMessageBox::Save:
            save_current_document(docfile_manager);
            return true;
        case QMessageBox::Cancel:
            return false;
        default:
            return false;
        }
    }
    return true;
}

void TextEditor::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_K) {
        QJsonObject hover_request = LSP::LSPMessages::hover(
            QUrl::fromLocalFile(file->fileName()), 0, 0);

        lsp->dispatch_request(hover_request);
    }
    QTextEdit::keyPressEvent(event);
}

void TextEditor::focusInEvent(QFocusEvent *e) {
    QTextEdit::focusInEvent(e);
    emit focused(*this);
}

const QFile *TextEditor::get_file() const { return file; }

} // namespace OpenConfigEditor
