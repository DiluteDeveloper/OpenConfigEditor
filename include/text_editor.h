#pragma once

#include "lsp/lsp_client.h"
#include "status_bar.h"
#include <QFile>
#include <QPlainTextEdit>

namespace OpenConfigEditor {

class TextEditor : public QPlainTextEdit {
public:
  TextEditor(StatusBar &status_bar);
  void open_file(StatusBar &status_bar);
  void new_file(StatusBar &status_bar);
  void save_file(StatusBar &status_bar);

  const QFile &get_file() const;
  const QString &get_file_path() const;

private:
  LSP::LSPClient* lsp;
  QFile *current_file;
  QString current_file_path;

  void try_save(StatusBar &status_bar);
  void keyPressEvent(QKeyEvent* event) override;
};

} // namespace OpenConfigEditor
