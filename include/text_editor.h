#pragma once

#include "lsp_client.h"
#include <QFile>
#include <QPlainTextEdit>

namespace OpenConfigEditor {

class TextEditor : public QPlainTextEdit {
    Q_OBJECT
  public:
    TextEditor(QWidget *parent = nullptr);
    void open_file();
    void new_file();
    void save_file();

    const QFile &get_file() const;
    const QString &get_file_path() const;

  signals:
    void file_opened(const TextEditor &text_editor);
    void file_saved(const TextEditor &text_editor);
    void file_created(const TextEditor &text_editor);

  private:
    LSP::LSPClient *lsp;
    QFile *current_file;
    QString current_file_path;

    void try_save();
    void keyPressEvent(QKeyEvent *event) override;
};

} // namespace OpenConfigEditor
