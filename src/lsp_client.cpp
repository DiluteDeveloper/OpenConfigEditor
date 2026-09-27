#include <QFileInfo>
#include <QJsonObject>

#include "lsp_client.h"
#include "qcoreapplication.h"

namespace OpenConfigEditor {

namespace LSP {

    const std::unordered_map<LSPMessages::MethodID, QString>
        LSPMessages::method_names = {
            {LSPMessages::MethodID::INITIALIZE, "initialize"},
            {LSPMessages::MethodID::HOVER, "textDocument/hover"},
            {LSPMessages::MethodID::DID_OPEN, "textDocument/didOpen"},
    };

    LSPClient::LSPClient(QObject *parent) : QObject(parent) {

        process = new QProcess(this);
        process->start("clangd");

        connect(process, &QProcess::readyReadStandardOutput, this, [this]() {
            buffer += process->readAllStandardOutput();
            try_parse_response();
        });

        // temporary
        response_handlers.emplace(LSPMessages::MethodID::HOVER, test_handler);

        // temporary
        response_handlers.emplace(LSPMessages::MethodID::INITIALIZE,
                                  test_handler);
        // temporary
        response_handlers.emplace(LSPMessages::MethodID::DID_OPEN,
                                  test_handler);

        QJsonObject initialize_request =
            LSPMessages::initialize(QCoreApplication::applicationPid(),
                                    LSPMessages::client_capabilities());
        dispatch_request(initialize_request);
    }
    void LSPClient::dispatch_request(const QJsonObject &request) const {

        qDebug() << "Dispatching LSP request...";

        // QByteArray hover_request = LSPRequest::hover(
        //     QUrl::fromLocalFile(text_editor.get_file_path()).toString(), 0,
        //     0);

        QByteArray request_bytes =
            QByteArray(QJsonDocument(request).toJson(QJsonDocument::Compact));

        QByteArray header_bytes = LSPMessages::header(request_bytes);

        qDebug().noquote().nospace()
            << "\033[31m"
            << "Request:\n"
            << QJsonDocument(request).toJson(QJsonDocument::Indented)
            << "\033[0m";

        process->write(header_bytes);
        process->write(request_bytes);
        qDebug() << "Dispatched LSP request";
        qDebug().noquote().nospace()
            << "\033[38;2;255;0;0m" << process->readAllStandardError()
            << "\033[0m";
    }
    void LSPClient::try_parse_response() {
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
                    content_length =
                        line.mid(strlen("Content-Length:")).trimmed().toInt();
                }
                // ignoring Content-Type — we only ever get utf-8 JSON in
                // practice
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
                response_handlers.at(static_cast<LSPMessages::MethodID>(
                    body_json.value("id").toInt()))(body_json);
            }

            buffer.remove(0, body_end);
        }
    }
    void LSPClient::test_handler(const QJsonObject &object) {
        qDebug().noquote().nospace()
            << "\033[33m"
            << "Response:\n"
            << QJsonDocument(object).toJson(QJsonDocument::Indented)
            << "\033[0m";
    }

    QByteArray LSPMessages::header(const QByteArray &json_message) {
        return QByteArray("Content-Length: " +
                          QByteArray::number(json_message.size()) + "\r\n\r\n");
    }
    QJsonObject LSPMessages::hover(const QUrl &file_uri, int32_t line,
                                   int32_t character) {
        QJsonObject request;
        request["textDocument"] = QJsonObject{{"uri", file_uri.toString()}};
        request["position"] =
            QJsonObject{{"line", line}, {"character", character}};

        return wrap_request(MethodID::HOVER, request);
    }

    QJsonObject LSPMessages::did_open(const QUrl &file_uri,
                                      const QString &language_id,
                                      int32_t version,
                                      const QString &contents) {
        QJsonObject text_document;
        text_document["uri"] = file_uri.toString();
        text_document["languageId"] = language_id;
        text_document["version"] = version;
        text_document["text"] = contents;
        return wrap_notification(MethodID::DID_OPEN,
                                 QJsonObject{{"textDocument", text_document}});
    }

    // client_locale: https://en.wikipedia.org/wiki/IETF_language_tag
    QJsonObject LSPMessages::initialize(
        int32_t process_id, const QJsonObject &client_capabilities,
        const QUrl *root_uri, const QString *client_name,
        const QString *client_version, const QString *client_locale,
        const QString *root_path, TraceValue *trace_value,
        const QJsonObject *workspace_folders) {
        qDebug() << "Constructing LSP initialisation request";

        QJsonObject request;
        request["processId"] = process_id;
        if (client_name != nullptr) {
            QJsonObject client_info{{"name", *client_name}};
            if (client_version != nullptr)
                client_info["version"] = *client_version;
            request["clientInfo"] = client_info;
        }
        if (client_locale != nullptr)
            request["locale"] = *client_locale;
        if (root_path != nullptr)
            request["rootPath"] = *root_path;

        root_uri != nullptr ? request["rootUri"] = root_uri->toString()
                            : request["rootUri"] = QJsonValue();

        // initialization requests 'initializationOptions' is left out for
        // now
        request["capabilities"] = client_capabilities;
        if (trace_value != nullptr)
            request["trace"] = trace_value_to_string(*trace_value);
        if (workspace_folders != nullptr)
            request["workspaceFolders"] = *workspace_folders;

        return wrap_request(MethodID::INITIALIZE,
                            request); //.toJson(QJsonDocument::Compact);
    }

    QString LSPMessages::trace_value_to_string(TraceValue trace_value) {
        switch (trace_value) {
        case OFF:
            return "off";
        case MESSAGES:
            return "messages";
        case VERBOSE:
            return "verbose";
        default:
            qWarning() << "Unrecognised trace value! defaulting to \"off\"";
            return "off";
        }
    }
    QJsonObject LSPMessages::client_capabilities() { return QJsonObject(); }

    QJsonObject LSPMessages::wrap_request(LSPMessages::MethodID method,
                                          QJsonObject request) {
        return QJsonObject{{"jsonrpc", "2.0"},
                           {"id", method},
                           {"method", method_names.at(method)},
                           {"params", request}};
    }
    QJsonObject LSPMessages::wrap_notification(LSPMessages::MethodID method,
                                               QJsonObject request) {
        return QJsonObject{{"jsonrpc", "2.0"},
                           {"method", method_names.at(method)},
                           {"params", request}};
    }
} // namespace LSP

} // namespace OpenConfigEditor
