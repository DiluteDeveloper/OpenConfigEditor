// #include "lsp/lsp_requests.h"
//
// #include <QJsonDocument>
//
// namespace OpenConfigEditor {
//
// namespace LSP {
//
// QByteArray LSPRequest::header(const QByteArray &json_message) {
//   return QByteArray("Content-Length: " +
//                     QByteArray::number(json_message.size()) + "\r\n\r\n");
// }
// QByteArray LSPRequest::hover(const QString &file_uri, int32_t line,
//                              int32_t character) {
//   qDebug() << file_uri;
//   return QJsonDocument(
//              QJsonObject{{"jsonrpc", "2.0"},
//                          {"id", RequestID::HOVER},
//                          {"method", "textDocument/hover"},
//                          {"params",
//                           QJsonObject{{"textDocument", file_uri},
//                                       {"position", QJsonObject{{"line",
//                                       line},
//                                                                {"character",
//                                                                 character}}}}}})
//       .toJson(QJsonDocument::Compact);
// }
//
// // client_locale: https://en.wikipedia.org/wiki/IETF_language_tag
// QByteArray LSPRequest::initialize(
//     int32_t process_id, const QString *client_name,
//     const QString *client_version, const QString *client_locale,
//     const QString *root_path, const QUrl &root_uri,
//     const QJsonObject &client_capabilities, TraceValue trace_value,
//     const QJsonObject *workspace_folders) {
//
//   qDebug() << "Constructing LSP initialisation request";
//
//   QJsonObject request;
//   request["processId"] = process_id;
//   if (client_name != nullptr) {
//     QJsonObject client_info{{"name", *client_name}};
//     if (client_version != nullptr)
//       client_info["version"] = *client_version;
//     request["clientInfo"] = client_info;
//   }
//   if (client_locale != nullptr)
//     request["locale"] = *client_locale;
//   if (root_path != nullptr)
//     request["rootPath"] = *root_path;
//   request["rootUri"] = root_uri.toString();
//   // initialization requests 'initializationOptions' is left out for
//   // now
//
//   return QJsonDocument(request).toJson(QJsonDocument::Compact);
// }
//
// QString LSPRequest::trace_value_to_string(TraceValue trace_value) {
//   switch (trace_value) {
//   case OFF:
//     return "off";
//   case MESSAGES:
//     return "messages";
//   case VERBOSE:
//     return "verbose";
//   }
// }
// } // namespace LSP
//   //
// } // namespace OpenConfigEditor
