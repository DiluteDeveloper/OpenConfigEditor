#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QtLogging>
#include <QKeyEvent>

#include "qmessagebox.h"
#include "status_bar.h"
#include "text_editor.h"

namespace OpenConfigEditor {

TextEditor::TextEditor(StatusBar &status_bar) {

  lsp = new LSP::LSPClient(this);
  current_file = new QFile(this);

  status_bar.set_file_name("New File");
  // connect(this, &QPlainTextEdit::textChanged, this,
  //         [this]() { on_text_changed(lsp); });
}

void TextEditor::open_file(StatusBar &status_bar) {
  try_save(status_bar);

  current_file_path =
      QFileDialog::getOpenFileName(this, "Open File", "/home/dilute");

  qDebug() << "Opening file:" << current_file_path;

  current_file->setFileName(current_file_path);
  if (!current_file->open(QIODevice::ReadOnly | QIODevice::Text)) {
    qWarning() << "Failed to open file" << current_file_path;
    return;
  }
  setPlainText(QTextStream(current_file).readAll());
  current_file->close();

#ifdef FULL_PATH_IN_STATUS_BAR
  status_bar.set_file_name(current_file_path);
#else
  status_bar.set_file_name(
      current_file.filesystemFileName().filename().c_str());
#endif

  qDebug() << "Successfully opened file:" << current_file_path;
}

void TextEditor::new_file(StatusBar &status_bar) {
  try_save(status_bar);

  current_file_path = "";
  current_file->setFileName("");
  status_bar.set_file_name("New File");
  clear();
}

void TextEditor::save_file(StatusBar &status_bar) {

  if (current_file->fileName().isEmpty()) {
    current_file_path =
        QFileDialog::getSaveFileName(this, "Save File", QDir::homePath(),
                                     "Text Files (*.tt);; All Files (*)");
    current_file->setFileName(current_file_path);
#ifdef FULL_PATH_IN_STATUS_BAR
    status_bar.set_file_name(current_file_path);
#else
    status_bar.set_file_name(
        current_file.filesystemFileName().filename().c_str());
#endif
  }
  qDebug() << "Saving file:" << current_file_path;

  if (!current_file->open(QIODevice::WriteOnly | QIODevice::Text)) {
    qWarning() << "Failed to open file:" << current_file_path;
    return;
  }
  QTextStream out(current_file);
  out << toPlainText();
  current_file->close();

  qDebug() << "Successfully saved file:" << current_file_path;

  status_bar.set_bytes_written(toPlainText().toUtf8().size());
  document()->setModified(false);
}

const QFile &TextEditor::get_file() const { return *current_file; }
const QString &TextEditor::get_file_path() const { return current_file_path; }

void TextEditor::try_save(StatusBar &status_bar) {

  if (document()->isModified()) {
    switch (QMessageBox::question(this, "Your file has not been saved.",
                                  "Do you want to save your changes?",
                                  QMessageBox::Save | QMessageBox::Discard |
                                      QMessageBox::Cancel)) {
    case QMessageBox::Cancel:
      return;
    case QMessageBox::Save:
      save_file(status_bar);
      break;
    default:
      break;
    }
  }
}

void TextEditor::keyPressEvent(QKeyEvent* event) {
  if(event->key() == Qt::Key_K) {
  QByteArray hover_request = LSP::LSPRequest::hover(
      QUrl::fromLocalFile(current_file_path).toString(), 0, 0);

  lsp->dispatch_request(hover_request);
  
  }
  QPlainTextEdit::keyPressEvent(event);

}

} // namespace OpenConfigEditor
