#pragma once

#include "qabstractitemmodel.h"
#include <QTreeView>

#include <expected>

namespace OpenConfigEditor {

namespace ProjectTreeModel {
    enum class ProjectTreeModelError {};
    extern std::expected<QAbstractItemModel *, ProjectTreeModelError>
    create_model_from_project_tree_json(const QJsonObject &project_tree_json);
    extern std::expected<QJsonObject *, ProjectTreeModelError>
    create_project_tree_json_from_model(const QAbstractItemModel &model);
}; // namespace ProjectTreeModel

class FileTree : public QTreeView {};

} // namespace OpenConfigEditor
