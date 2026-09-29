#pragma once

#include "qdir.h"
#include "qtextdocument.h"
#include <QObject>

namespace OpenConfigEditor {

struct DocumentFile : public QObject {
  public:
    // Opens file and puts file content into document
    DocumentFile(const QString &file_path, QObject *parent = nullptr);
    // Does not open file, and puts content into document
    DocumentFile(const QString &file_path, const QString &content,
                 QObject *parent = nullptr);
    QTextDocument document;
    QFile file;

    // Used as a simplified shared pointer as
    // the QPlainTextEdit document stores a pointer
    // to this document, not a shared pointer.
    uint8_t reference_count = 1;
};

// Unified document file storage class.
// I'd like to enunciate that this class is NOT
// responsible for modifying the documents whatsoever
// past loading the corresponding file content into
// the document from disk
class DocumentFileManager : public QObject {
  public:
    DocumentFileManager(QObject *parent = nullptr);

    // @return nullptr if the file fails to open
    DocumentFile *load_or_get_document(const QString &file_path);
    // @return nullptr if the file fails to open
    // This function creates a corresponding DocumentFile if it
    // doesn't already exist, if, for instance, this is a scratch
    // document being worked on that the user would like to save
    DocumentFile *save_document(const QString &file_path,
                                const QString &content);

    // Decrements reference count until 0, then deletes,
    // so multiple buffers can load or unload
    // the same document without risk of destroying a
    // document that is still being used in another buffer
    void unload_document(const QString &file_path);

  private:
    // Not reallocated like vector so pointers to elements are valid
    // The purpose of this is to allow multiple buffers to have the same
    // document open at the same time, and to ensure that each time
    // it is opened it does not have to be loaded from disk again.
    // While this does mean 'shared mutable state', practically
    // speaking, they should never be edited at the same time
    std::unordered_map<QString, DocumentFile> documents;
};
} // namespace OpenConfigEditor
