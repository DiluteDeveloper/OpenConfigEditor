#pragma once

#include "qdir.h"
#include "qjsondocument.h"

namespace OpenConfigEditor {

namespace JSON {
    std::optional<QJsonDocument> load_from_file(QFile &file);

} // namespace JSON
} // namespace OpenConfigEditor
