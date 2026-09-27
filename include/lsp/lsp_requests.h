// #pragma once
//
// #include <QJsonObject>
//
// namespace OpenConfigEditor {
//
// namespace LSP {
//
// // Helper class to send LSP requests based on the
// // LSP specification found here:
// //
// https://microsoft.github.io/language-server-protocol/specifications/lsp/3.17/specification/
// // #initialize
//
// class LSPRequest {
//
// public:
//   enum RequestID {
//     HOVER,
//   };
//   enum TraceValue { OFF, MESSAGES, VERBOSE };
//
//   static QByteArray header(const QByteArray &json_message);
//
//   static QByteArray hover(const QString &file_uri, int32_t line,
//                           int32_t character);
//
//   static QByteArray initialize(int32_t process_id, const QString
//   *client_name,
//                                const QString *client_version,
//                                const QString *client_locale,
//                                const QString *root_path, const QUrl
//                                &root_uri, const QJsonObject
//                                &client_capabilities, TraceValue trace_value,
//                                const QJsonObject *workspace_folders);
//
// private:
//   QString trace_value_to_string(TraceValue trace_value);
// };
//
// }; // namespace LSP
//
// } // namespace OpenConfigEditor
