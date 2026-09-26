#pragma once

#include <QProcess>

#include "lsp/lsp_requests.h"

namespace OpenConfigEditor {

namespace LSP {

class LSPClient : public QObject {

public:
  LSPClient(QObject *parent);
  void dispatch_request(const QByteArray &request) const;
  void try_parse_response();

  static void test_handler(const QJsonObject &object);

private:
  QProcess *process;
  QByteArray buffer;

  std::unordered_map<LSPRequest::RequestID,
                     std::function<void(const QJsonObject &)>>
      response_handlers;
};

} // namespace LSP

} // namespace OpenConfigEditor

// class LspClient {
// public:
//   LspClient();
//   ~LspClient() override;
//
//   // Launches the language server binary, e.g. "clangd" with args
//   // like {"--background-index"}.
//   void start(const QString &program, const QStringList &args = {});
//
//   // Sends the LSP "initialize" handshake. rootUri should be a
//   // file:// URI for your project root.
//   void initialize(const QString &rootUri);
//
//   // --- Document sync ---
//   void didOpen(const QString &uri, const QString &languageId, int version,
//                const QString &text);
//   void didChange(const QString &uri, int version, const QString &fullText);
//   void didClose(const QString &uri);
//
//   // --- Requests ---
//   // Each takes a callback invoked with the raw "result" field of the
//   // response once it arrives. Errors are logged and passed as an
//   // empty QJsonValue to the callback.
//   using ResultCallback = std::function<void(const QJsonValue &result)>;
//
//   void completion(const QString &uri, int line, int character,
//                   ResultCallback callback);
//   void definition(const QString &uri, int line, int character,
//                   ResultCallback callback);
//   void hover(const QString &uri, int line, int character,
//              ResultCallback callback);
//
// signals:
//   // Fired whenever the server pushes textDocument/publishDiagnostics.
//   void diagnosticsReceived(const QString &uri, const QJsonArray
//   &diagnostics);
//
//   // Fired if the server writes to stderr (logs, crashes, etc).
//   void serverError(const QString &message);
//
//   // Fired once initialize/initialized handshake completes.
//   void initialized();
//
// private slots:
//   void onReadyReadStdout();
//   void onReadyReadStderr();
//   void onProcessError(QProcess::ProcessError error);
//
// private:
//   void sendMessage(const QJsonObject &message);
//   int sendRequest(const QString &method, const QJsonObject &params,
//                   ResultCallback callback);
//   void sendNotification(const QString &method, const QJsonObject &params);
//   void tryParseBuffer();
//   void dispatch(const QJsonObject &message);
//
//   QProcess *m_process = nullptr;
//   QByteArray m_buffer;
//   int m_nextId = 1;
//   QMap<int, ResultCallback> m_pending;
// };
