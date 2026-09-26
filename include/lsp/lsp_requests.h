#pragma once

#include <QJsonObject>

namespace OpenConfigEditor {

namespace LSP {

class LSPRequest {

public:
  enum RequestID {
    HOVER,
  };

  static QByteArray header(const QByteArray &json_message);

  static QByteArray hover(const QString &file_uri, int32_t line,
                          int32_t character);
};

}; // namespace LSP

} // namespace OpenConfigEditor
