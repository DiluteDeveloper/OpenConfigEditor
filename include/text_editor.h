#pragma once

#include "document_file_manager.h"
#include "lsp_client.h"
#include <QFile>
#include <QTextEdit>

namespace OpenConfigEditor {

class TextEditor : public QTextEdit {
    Q_OBJECT
  public:
    TextEditor(QWidget *parent = nullptr);
    void on_user_request_open_file(DocumentFileManager &docfile_manager);
    void
    on_user_request_create_new_document(DocumentFileManager &docfile_manager);
    void
    on_user_request_save_current_document(DocumentFileManager &docfile_manager);

    const QFile *get_file() const;

  signals:
    void file_opened(const QFile &file);
    void document_saved(const TextEditor &text_editor);
    void document_created();
    void focused(const TextEditor &text_editor);

  private:
    LSP::LSPClient *lsp = nullptr;
    QFile *file = nullptr;

    // @return true when user would like to continue with the
    // action, or false when the user has cancelled the
    // interaction
    bool
    request_user_save_current_document(DocumentFileManager &docfile_manager);
    void keyPressEvent(QKeyEvent *event) override;
    void focusInEvent(QFocusEvent *e) override;
};

} // namespace OpenConfigEditor
