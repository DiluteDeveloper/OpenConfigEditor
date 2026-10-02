#include "io/json.h"
#include "qjsonarray.h"

namespace OpenConfigEditor {

namespace JSON {

    std::optional<QJsonDocument> load_from_file(QFile &file) {

        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "Failed to open " << file.fileName() << ":"
                       << file.errorString();
            return std::nullopt;
        }

        const QByteArray contents = file.readAll();
        file.close();

        QJsonParseError parse_error;
        const QJsonDocument document =
            QJsonDocument::fromJson(contents, &parse_error);
        if (parse_error.error != QJsonParseError::NoError) {
            qWarning() << "Invalid JSON in" << file.fileName() << ":"
                       << parse_error.errorString();
            return std::nullopt;
        }

        return document;
    }
} // namespace JSON
} // namespace OpenConfigEditor
