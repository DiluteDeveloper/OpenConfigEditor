#pragma once

#include "qjsondocument.h"
#include <QProcess>

namespace OpenConfigEditor {

namespace LSP {

    // Helper class to send LSP requests based on the
    // LSP specification found here:
    //
    // https://microsoft.github.io/language-server-protocol/specifications/lsp/3.17/specification/
    class LSPRequests {

      public:
        enum MethodID {
            INITIALIZE,
            HOVER,
        };
        enum TraceValue { OFF, MESSAGES, VERBOSE };

        static QByteArray header(const QByteArray &json_message);

        static QJsonObject hover(const QString &file_uri, int32_t line,
                                 int32_t character);

        static QJsonObject
        initialize(int32_t process_id, const QJsonObject &client_capabilities,
                   const QUrl *root_uri = nullptr,
                   const QString *client_name = nullptr,
                   const QString *client_version = nullptr,
                   const QString *client_locale = nullptr,
                   const QString *root_path = nullptr,
                   TraceValue *trace_value = nullptr,
                   const QJsonObject *workspace_folders = nullptr);

        static QJsonObject client_capabilities();

      private:
        static QString trace_value_to_string(TraceValue trace_value);
        static QJsonObject wrap_request(MethodID method, QJsonObject request);

        static const std::unordered_map<MethodID, QString> method_names;
    };

    class LSPClient : public QObject {

      public:
        LSPClient(QObject *parent);
        void dispatch_request(const QJsonObject &request) const;
        void try_parse_response();

        static void test_handler(const QJsonObject &object);

      private:
        QProcess *process;
        QByteArray buffer;

        std::unordered_map<LSPRequests::MethodID,
                           std::function<void(const QJsonObject &)>>
            response_handlers;
    };

} // namespace LSP

} // namespace OpenConfigEditor
