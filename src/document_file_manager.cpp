#include "document_file_manager.h"
#include "qplaintextedit.h"
#include <QSaveFile>
#include <QTextDocumentWriter>

namespace OpenConfigEditor {

constexpr const char *FILE_FAILED_TO_OPEN_FILENAME_INDICATOR = "FAILED_TO_OPEN";

DocumentFile::DocumentFile(const QString &file_path, QObject *parent)
    : QObject(parent), document(this), file(file_path, this) {
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file: " << file.filesystemFileName();
        file.setFileName(FILE_FAILED_TO_OPEN_FILENAME_INDICATOR);
        return;
    }
    document.setPlainText(QTextStream(&file).readAll());
    document.setDocumentLayout(new QPlainTextDocumentLayout(&document));
    document.setModified(false);
    file.close();
}
DocumentFile::DocumentFile(const QString &file_path, const QString &content,
                           QObject *parent)
    : QObject(parent), document(content, this), file(file_path, this) {
    document.setDocumentLayout(new QPlainTextDocumentLayout(&document));
}

DocumentFileManager::DocumentFileManager(QObject *parent) : QObject(parent) {}

DocumentFile *
DocumentFileManager::load_or_get_document(const QString &file_path) {

    auto [it, inserted] = documents.try_emplace(file_path, file_path, this);

    if (!inserted)
        it->second.reference_count++;

    if (it->second.file.fileName() == FILE_FAILED_TO_OPEN_FILENAME_INDICATOR) {
        documents.erase(FILE_FAILED_TO_OPEN_FILENAME_INDICATOR);
        return nullptr;
    }
    qDebug() << file_path << "reference count:" << it->second.reference_count;
    return &it->second;
}

DocumentFile *DocumentFileManager::save_document(const QString &file_path,
                                                 const QString &content) {

    auto [it, inserted] =
        documents.try_emplace(file_path, file_path, content, this);

    QSaveFile out(file_path);
    if (!out.open(QIODevice::WriteOnly)) {
        qWarning() << "save: open failed:" << file_path << out.errorString();
        return nullptr;
    }
    out.write(it->second.document.toPlainText().toUtf8());
    if (!out.commit()) {
        qWarning() << "save: commit failed:" << file_path << out.errorString();
        return nullptr;
    }

    return &it->second;
}

void DocumentFileManager::unload_document(const QString &file_path) {
    if (documents.contains(file_path)) {
        DocumentFile &df = documents.at(file_path);

        df.reference_count--;
        qDebug() << file_path << "reference count:" << df.reference_count;
        if (df.reference_count == 0) {
            documents.erase(file_path);
            qDebug() << "Unloaded document:" << file_path;
        }
    }
}
} // namespace OpenConfigEditor
