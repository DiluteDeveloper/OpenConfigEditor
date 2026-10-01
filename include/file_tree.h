#pragma once

#include "qabstractitemmodel.h"
#include <QTreeView>

#include <expected>

namespace OpenConfigEditor {

namespace ProjectTreeModel {
    enum class ModelCreationError {
        NAME_AND_DIRECTORY_MISSING,

    };
    extern std::expected<QAbstractItemModel *, ModelCreationError>
    create_model_from_project_tree_json(const QJsonObject &project_tree_json);
}; // namespace ProjectTreeModel

class FileTree : public QTreeView {};

} // namespace OpenConfigEditor
