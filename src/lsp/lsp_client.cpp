#include <QFileInfo>
#include <QJsonObject>

#include "lsp/lsp_client.h"
#include "lsp/lsp_requests.h"

namespace OpenConfigEditor {

namespace LSP {

LSPClient::LSPClient(QObject *parent) : QObject(parent) {

  process = new QProcess(this);
  process->start("clangd");

  connect(process, &QProcess::readyReadStandardOutput, this, [this]() {
    buffer += process->readAllStandardOutput();
    try_parse_response();
  });

  response_handlers.emplace(LSPRequest::RequestID::HOVER, test_handler);
}
void LSPClient::dispatch_request(const QByteArray &request) const {

  qDebug() << process->readAllStandardError();
  qDebug() << "Dispatching request to LSP...";

  // QByteArray hover_request = LSPRequest::hover(
  //     QUrl::fromLocalFile(text_editor.get_file_path()).toString(), 0, 0);
  QByteArray header = LSPRequest::header(request);

  qDebug() << "Header:" << header;
  qDebug() << "Request:" << request;
  process->write(header);
  process->write(request);
  qDebug() << "Finished dispatching request to LSP...";
}
void LSPClient::try_parse_response() {
  qDebug() << "Parsing response...";
  while (true) {

    int header_end = buffer.indexOf("\r\n\r\n");
    if (header_end < 0) {
      return;
    }

    QByteArray header_block = buffer.left(header_end);
    int content_length = -1;

    const QList<QByteArray> lines = header_block.split('\n');
    for (QByteArray line : lines) {
      line = line.trimmed();
      if (line.startsWith("Content-Length:")) {
        content_length = line.mid(strlen("Content-Length:")).trimmed().toInt();
      }
      // ignoring Content-Type — we only ever get utf-8 JSON in practice
    }

    if (content_length < 0) {
      qWarning() << "Malformed message: no Content-Length header";
      buffer.clear();
      return;
    }

    int body_start = header_end + 4; // skip past "\r\n\r\n"
    int body_end = body_start + content_length;

    if (buffer.size() < body_end) {
      return; // body not fully received yet
    }

    QByteArray body = buffer.mid(body_start, content_length);

    QJsonParseError err;
    QJsonDocument body_doc = QJsonDocument::fromJson(body, &err);

    if (err.error != QJsonParseError::NoError) {
      qWarning() << "JSON parse error:" << err.errorString();
    } else if (body_doc.isObject()) {
      QJsonObject body_json = body_doc.object();
      response_handlers[static_cast<LSPRequest::RequestID>(
          body_json.value("id").toInt())](body_json);
    }

    buffer.remove(0, body_end);
  }
}
void LSPClient::test_handler(const QJsonObject &object) { qDebug() << object; }

} // namespace LSP

} // namespace OpenConfigEditor

// LspClient::LspClient(QObject *parent) : QObject(parent) {
//   m_process = new QProcess(this);
//
//   connect(m_process, &QProcess::readyReadStandardOutput, this,
//           &LspClient::onReadyReadStdout);
//   connect(m_process, &QProcess::readyReadStandardError, this,
//           &LspClient::onReadyReadStderr);
//   connect(m_process, &QProcess::errorOccurred, this,
//           &LspClient::onProcessError);
// }
//
// LspClient::~LspClient() {
//   if (m_process->state() != QProcess::NotRunning) {
//     m_process->terminate();
//     m_process->waitForFinished(1000);
//   }
// }
//
// void LspClient::start(const QString &program, const QStringList &args) {
//   m_process->start(program, args);
//   if (!m_process->waitForStarted(3000)) {
//     emit serverError(
//         QStringLiteral("Failed to start language server: %1").arg(program));
//   }
// }

// ---------------------------------------------------------------------
// Framing: every LSP message is
//   Content-Length: <N>\r\n
//   \r\n
//   <N bytes of JSON>
// ---------------------------------------------------------------------

// void LspClient::sendMessage(const QJsonObject &message) {
//   QByteArray body = QJsonDocument(message).toJson(QJsonDocument::Compact);
//   QByteArray header =
//       "Content-Length: " + QByteArray::number(body.size()) + "\r\n\r\n";
//   m_process->write(header);
//   m_process->write(body);
// }
//
// int LspClient::sendRequest(const QString &method, const QJsonObject &params,
//                            ResultCallback callback) {
//   int id = m_nextId++;
//   QJsonObject msg{
//       {"jsonrpc", "2.0"}, {"id", id}, {"method", method}, {"params",
//       params}};
//   if (callback)
//     m_pending.insert(id, callback);
//   sendMessage(msg);
//   return id;
// }
//
// void LspClient::sendNotification(const QString &method,
//                                  const QJsonObject &params) {
//   QJsonObject msg{{"jsonrpc", "2.0"}, {"method", method}, {"params",
//   params}}; sendMessage(msg);
// }
//
// // ---------------------------------------------------------------------
// // Handshake
// // ---------------------------------------------------------------------
//
// void LspClient::initialize(const QString &rootUri) {
//   QJsonObject capabilities; // fill in as you add features
//   QJsonObject textDocumentCaps;
//   QJsonObject completionCaps;
//   completionCaps["dynamicRegistration"] = false;
//   textDocumentCaps["completion"] = completionCaps;
//   capabilities["textDocument"] = textDocumentCaps;
//
//   QJsonObject params{{"processId", QCoreApplication::applicationPid()},
//                      {"rootUri", rootUri},
//                      {"capabilities", capabilities}};
//
//   sendRequest("initialize", params, [this](const QJsonValue &) {
//     sendNotification("initialized", QJsonObject{});
//     emit initialized();
//   });
// }
//
// // ---------------------------------------------------------------------
// // Document sync
// // ---------------------------------------------------------------------
//
// void LspClient::didOpen(const QString &uri, const QString &languageId,
//                         int version, const QString &text) {
//   QJsonObject textDocument{{"uri", uri},
//                            {"languageId", languageId},
//                            {"version", version},
//                            {"text", text}};
//   sendNotification("textDocument/didOpen",
//                    QJsonObject{{"textDocument", textDocument}});
// }
//
// void LspClient::didChange(const QString &uri, int version,
//                           const QString &fullText) {
//   // Uses full-document sync (simplest correct option). Incremental
//   // sync is possible but needs range tracking - add later if the
//   // server benefits from it (most are fine with full sync for
//   // moderate file sizes).
//   QJsonObject textDocument{{"uri", uri}, {"version", version}};
//   QJsonObject change{{"text", fullText}};
//   QJsonArray changes{change};
//
//   QJsonObject params{{"textDocument", textDocument},
//                      {"contentChanges", changes}};
//   sendNotification("textDocument/didChange", params);
// }
//
// void LspClient::didClose(const QString &uri) {
//   QJsonObject textDocument{{"uri", uri}};
//   sendNotification("textDocument/didClose",
//                    QJsonObject{{"textDocument", textDocument}});
// }
//
// // ---------------------------------------------------------------------
// // Requests
// // ---------------------------------------------------------------------
//
// static QJsonObject positionParams(const QString &uri, int line, int
// character) {
//   QJsonObject textDocument{{"uri", uri}};
//   QJsonObject position{{"line", line}, {"character", character}};
//   return QJsonObject{{"textDocument", textDocument}, {"position", position}};
// }
//
// void LspClient::completion(const QString &uri, int line, int character,
//                            ResultCallback callback) {
//   sendRequest("textDocument/completion", positionParams(uri, line,
//   character),
//               callback);
// }
//
// void LspClient::definition(const QString &uri, int line, int character,
//                            ResultCallback callback) {
//   sendRequest("textDocument/definition", positionParams(uri, line,
//   character),
//               callback);
// }
//
// void LspClient::hover(const QString &uri, int line, int character,
//                       ResultCallback callback) {
//   sendRequest("textDocument/hover", positionParams(uri, line, character),
//               callback);
// }
//
// // ---------------------------------------------------------------------
// // Incoming data
// // ---------------------------------------------------------------------
//
// void LspClient::onReadyReadStdout() {
//   m_buffer.append(m_process->readAllStandardOutput());
//   tryParseBuffer();
// }
//
// void LspClient::onReadyReadStderr() {
//   QByteArray err = m_process->readAllStandardError();
//   if (!err.isEmpty())
//     emit serverError(QString::fromUtf8(err));
// }
//
// void LspClient::onProcessError(QProcess::ProcessError error) {
//   emit serverError(QStringLiteral("Language server process error: %1")
//                        .arg(m_process->errorString()));
//   Q_UNUSED(error);
// }
//
// void LspClient::tryParseBuffer() {
//   // Loop in case multiple full messages arrived in one read.
//   while (true) {
//     int headerEnd = m_buffer.indexOf("\r\n\r\n");
//     if (headerEnd < 0)
//       return; // header not fully received yet
//
//     QByteArray header = m_buffer.left(headerEnd);
//     int lenPos = header.indexOf("Content-Length:");
//     if (lenPos < 0) {
//       // Malformed - drop what we have to avoid getting stuck.
//       m_buffer.clear();
//       return;
//     }
//
//     int numStart = lenPos + int(strlen("Content-Length:"));
//     int lineEnd = header.indexOf("\r\n", numStart);
//     if (lineEnd < 0)
//       lineEnd = header.size();
//     int contentLength =
//         header.mid(numStart, lineEnd - numStart).trimmed().toInt();
//
//     int bodyStart = headerEnd + 4;
//     if (m_buffer.size() < bodyStart + contentLength)
//       return; // body not fully received yet
//
//     QByteArray body = m_buffer.mid(bodyStart, contentLength);
//     m_buffer.remove(0, bodyStart + contentLength);
//
//     QJsonParseError parseError;
//     QJsonDocument doc = QJsonDocument::fromJson(body, &parseError);
//     if (parseError.error != QJsonParseError::NoError) {
//       emit serverError(QStringLiteral("Failed to parse LSP message: %1")
//                            .arg(parseError.errorString()));
//       continue;
//     }
//     dispatch(doc.object());
//   }
// }
//
// void LspClient::dispatch(const QJsonObject &message) {
//   // Response to one of our requests.
//   if (message.contains("id") &&
//       (message.contains("result") || message.contains("error"))) {
//     int id = message.value("id").toInt();
//     auto it = m_pending.find(id);
//     if (it != m_pending.end()) {
//       ResultCallback cb = it.value();
//       m_pending.erase(it);
//       if (message.contains("error")) {
//         emit serverError(
//             QStringLiteral("LSP error: %1")
//                 .arg(QString::fromUtf8(
//                     QJsonDocument(message.value("error").toObject())
//                         .toJson())));
//         cb(QJsonValue());
//       } else {
//         cb(message.value("result"));
//       }
//     }
//     return;
//   }
//
//   // Notification from the server.
//   if (message.contains("method")) {
//     QString method = message.value("method").toString();
//     QJsonObject params = message.value("params").toObject();
//
//     if (method == "textDocument/publishDiagnostics") {
//       QString uri = params.value("uri").toString();
//       QJsonArray diagnostics = params.value("diagnostics").toArray();
//       emit diagnosticsReceived(uri, diagnostics);
//     }
//     // Extend here for window/logMessage, window/showMessage, etc.
//   }
// }
