#pragma once

#include "qdir.h"
#include "qjsondocument.h"
#include "qstandarditemmodel.h"
#include <QJsonArray>
#include <QString>
#include <QTreeView>

namespace OpenConfigEditor {

namespace ProjectTreeModel {

    extern std::optional<QStandardItemModel *>
    create_model_from_project_tree_json(const QJsonArray &root_array);
    extern std::optional<QStandardItemModel *>
    create_model_from_project_tree_json_file(QFile &project_tree_json);

    extern std::optional<QJsonArray *>
    create_project_tree_json_array_from_model(const QStandardItemModel &model);
    // extern bool save_project_tree_json_array_file_from_model(
    //     const QStandardItemModel &model, QFile &file);
}; // namespace ProjectTreeModel

} // namespace OpenConfigEditor
