#include "lsp/lsp_requests.h"

#include <QJsonDocument>

namespace OpenConfigEditor {

namespace LSP {

QByteArray LSPRequest::header(const QByteArray &json_message) {
  return QByteArray("Content-Length: " +
                    QByteArray::number(json_message.size()) + "\r\n\r\n");
}
QByteArray LSPRequest::hover(const QString &file_uri, int32_t line,
                             int32_t character) {
  qDebug() << file_uri;
  return QJsonDocument(
             QJsonObject{{"jsonrpc", "2.0"},
                         {"id", RequestID::HOVER},
                         {"method", "textDocument/hover"},
                         {"params",
                          QJsonObject{{"textDocument", file_uri},
                                      {"position", QJsonObject{{"line", line},
                                                               {"character",
                                                                character}}}}}})
      .toJson(QJsonDocument::Compact);
}

} // namespace LSP
  //
} // namespace OpenConfigEditor
