#include "project_tree.h"
#include "io/json.h"
#include "qjsonarray.h"

#include <QDir>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonObject>
#include <QSet>
#include <QStandardItem>
#include <QStandardItemModel>

#include <deque>
#include <memory>

namespace {

const auto NAME_KEY = QStringLiteral("name");
const auto PATH_KEY = QStringLiteral("path");
const auto VIRTUAL_NODES_KEY = QStringLiteral("virtual_nodes");

const int PATH_ROLE = Qt::UserRole + 1;
const int ENTRY_PATH_ROLE = Qt::UserRole + 2;

} // namespace

namespace OpenConfigEditor {
namespace ProjectTreeModel {

    // Turns an array of json nodes into a heap allocated
    // QStandardItemModel.
    //
    // A node is either physical or virtual: a node with a "path" points
    // at a real directory on disk, and instead of holding children from
    // the JSON it gets children mirroring whatever is inside that
    // directory. A node without a "path" is virtual, so its children
    // come from its "virtual_nodes" array. A node cannot have both.
    // The "name" is optional, a physical node with no name gets the last
    // directory in its path as the name.
    //
    // The walk is iterative rather than recursive: the deque holds the
    // nodes still to visit, one entry per level of the tree, and each
    // entry remembers which item its nodes get appended to and how far
    // through the array it has gotten. Levels are pushed when their
    // parent is created and popped once their array is used up.
    // Held by a unique_ptr until the walk succeeds, otherwise the error
    // returns below would leak the model along with every item already
    // appended to it. Released back to a plain pointer on success.
    std::optional<QStandardItemModel *>
    create_model_from_project_tree_json(const QJsonArray &root_array) {
        static const QString error_prefix = QStringLiteral(
            "Failed to create project tree model from json array:");
        static const QString warning_prefix = QStringLiteral(
            "Warning while creating project tree model from json array:");

        std::unique_ptr<QStandardItemModel> model(new QStandardItemModel());

        struct PendingLevel {
            QJsonArray nodes;
            QStandardItem *parent;
            qsizetype next_index;
            QSet<QString> sibling_names;
        };

        // The invisible root item is the parent of the top level nodes, so
        // seeding the walk with it makes the root array just another level.
        std::deque<PendingLevel> pending;
        pending.push_back({root_array, model->invisibleRootItem(), 0, {}});

        while (!pending.empty()) {
            PendingLevel &current = pending.back();

            if (current.next_index >= current.nodes.size()) {
                pending.pop_back();
                continue;
            }

            const QJsonValue node_value =
                current.nodes.at(current.next_index++);
            if (!node_value.isObject()) {
                qWarning() << error_prefix << "Node is not a JSON object";
                return std::nullopt;
            }

            const QJsonObject node_object = node_value.toObject();

            // An absent name or path counts as unset, an explicit JSON null
            // is treated the same way as leaving the key out.
            const QJsonValue name_value = node_object.value(NAME_KEY);
            bool has_name = !name_value.isUndefined() && !name_value.isNull();
            if (has_name &&
                (!name_value.isString() || name_value.toString().isEmpty())) {
                qDebug() << warning_prefix
                         << "JSON project tree contains a node with an empty "
                            "name. Attempting to use path name instead";
                has_name = false;
            }

            const QJsonValue path_value = node_object.value(PATH_KEY);
            bool has_path = !path_value.isUndefined() && !path_value.isNull();
            if (has_path && !path_value.isString()) {

                qDebug() << warning_prefix
                         << "JSON project tree contains a node with an empty "
                            "path. Skipping node";
                continue;
            }

            const QJsonValue virtual_nodes_value =
                node_object.value(VIRTUAL_NODES_KEY);
            bool has_virtual_nodes = !virtual_nodes_value.isUndefined() &&
                                     !virtual_nodes_value.isNull();
            if (has_virtual_nodes && !virtual_nodes_value.isArray()) {
                qDebug() << warning_prefix
                         << "JSON project tree contains virtual nodes that are "
                            "not an array. Skipping node";
                continue;
            }
            if (has_path && has_virtual_nodes) {
                qDebug() << warning_prefix
                         << "JSON project tree contains a node with a path and "
                            "virtual nodes. These are mutually exclusive. "
                            "Falling back to path";
                has_virtual_nodes = false;
            }

            // Name falls back to the last directory of the path, and a node
            // with neither a name nor a path has nothing to label it with.
            QString node_name;
            if (has_name) {
                node_name = name_value.toString();
            } else if (has_path) {
                node_name = QFileInfo(QDir::cleanPath(path_value.toString()))
                                .fileName();
                if (node_name.isEmpty()) {
                    qDebug() << warning_prefix
                             << "JSON project tree contains a node with an "
                                "invalid path:"
                             << path_value.toString() << ". Skipping node";
                    continue;
                }
            } else {
                qDebug() << warning_prefix
                         << "JSON project tree contains a node that does not "
                            "have a name or a path. Skipping node";
                continue;
            }

            // A tree needs sibling names to be unique for a view to address
            // them, so duplicates are rejected before the item is made.
            if (current.sibling_names.contains(node_name)) {
                qDebug() << warning_prefix
                         << "Json project tree contains a node with duplicate "
                            "sibling names. Skipping node";
                continue;
            }
            current.sibling_names.insert(node_name);

            const QJsonArray node_children = virtual_nodes_value.toArray();
            QStandardItem *parent = current.parent;

            if (has_path) {
                // Physical node: a path that is missing, is not a directory,
                // or cannot be read is not fatal. The node is still shown,
                // with the path and the reason it is unusable added to its
                // name, and it just gets no children since there is nothing
                // on disk to mirror. The path stays in the item's data so it
                // can be pointed somewhere real.
                const QString node_path = path_value.toString();
                const QFileInfo path_info(node_path);

                QString invalid_reason;
                if (!path_info.exists())
                    invalid_reason = QStringLiteral("does not exist");
                else if (!path_info.isDir())
                    invalid_reason = QStringLiteral("is not a directory");
                else if (!path_info.isReadable())
                    invalid_reason = QStringLiteral("is not readable");

                const QString node_label =
                    invalid_reason.isEmpty()
                        ? node_name
                        : QStringLiteral("%1 [%2: %3]")
                              .arg(node_name, node_path, invalid_reason);

                auto *item = new QStandardItem(node_label);
                item->setData(node_path, PATH_ROLE);
                parent->appendRow(item);

                if (!invalid_reason.isEmpty())
                    continue;

                // Children come from disk instead of the JSON, each entry
                // keeping its full path so a view can open it later. Only
                // the immediate contents are added, deeper levels are left
                // to be filled in when they are opened.
                QDir directory(node_path);
                const QFileInfoList entries = directory.entryInfoList(
                    QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden,
                    QDir::Name);
                for (const QFileInfo &entry : entries) {
                    auto *entry_item = new QStandardItem(entry.fileName());
                    entry_item->setData(
                        directory.absoluteFilePath(entry.fileName()),
                        ENTRY_PATH_ROLE);
                    item->appendRow(entry_item);
                }
                continue;
            }

            // Virtual node: push a level for its children, which the walk
            // picks up next iteration.
            auto *item = new QStandardItem(node_name);
            parent->appendRow(item);
            if (!node_children.isEmpty())
                pending.push_back({node_children, item, 0, {}});
        }

        return model.release();
    }

    std::optional<QStandardItemModel *>
    create_model_from_project_tree_json_file(QFile &project_tree_json) {
        auto model_document_opt = JSON::load_from_file(project_tree_json);

        if (!model_document_opt.has_value())
            return std::nullopt;

        auto model_document = model_document_opt.value();

        if (!model_document.isArray()) {
            qWarning() << "Failed to create model from project tree json:"
                       << project_tree_json.fileName() << "is not an array";
            return std::nullopt;
        }
        auto model_opt =
            create_model_from_project_tree_json(model_document.array());

        if (!model_opt.has_value())
            return std::nullopt;

        return model_opt.value();
    }
    // The inverse of the function above: walks the model and produces
    // the array of json nodes that would build it back up.
    //
    // An item with a path in its data is physical, so it is written out
    // as a name and a path and its children are left out, they came
    // from disk and are not part of the JSON. Everything else is
    // virtual and gets its children written as "virtual_nodes".
    //
    // The walk is iterative again, one deque entry per level. Levels are
    // finished child first rather than parent first, because a node's
    // "virtual_nodes" array can only be filled in once its children
    // have been turned into json, so a level is popped when its parent
    // has no rows left and its node is then handed up to the level
    // below.
    std::optional<QJsonArray *>
    create_project_tree_json_array_from_model(const QStandardItemModel &model) {

        static const QString error_prefix = QStringLiteral(
            "Failed to create project tree json array from model:");
        static const QString warning_prefix = QStringLiteral(
            "Warning while creating project tree json array from model:");

        struct PendingLevel {
            QStandardItem *parent;
            qsizetype next_index;
            QSet<QString> sibling_names;
            QJsonArray child_nodes;
            QJsonObject node;
        };

        // Same trick as parsing, the invisible root item holds the top level
        // nodes, its collected children are the returned array.
        std::deque<PendingLevel> pending;
        pending.push_back({model.invisibleRootItem(), 0, {}, {}, {}});

        QJsonArray root_nodes;

        while (!pending.empty()) {
            PendingLevel &current = pending.back();

            if (current.next_index >= current.parent->rowCount()) {
                const QJsonObject finished_node = current.node;
                const QJsonArray finished_children = current.child_nodes;
                pending.pop_back();
                if (pending.empty()) {
                    root_nodes = finished_children;
                    continue;
                }
                QJsonObject completed_node = finished_node;
                completed_node.insert(VIRTUAL_NODES_KEY, finished_children);
                pending.back().child_nodes.append(completed_node);
                continue;
            }

            QStandardItem *item = current.parent->child(current.next_index, 0);
            current.next_index++;
            if (item == nullptr) {
                qDebug() << warning_prefix
                         << "Project tree model contains an unexpected missing "
                            "node. Skipping node";
                continue;
            }

            // Same uniqueness rule as parsing, otherwise the model could
            // describe a tree that the parser would reject.
            const QString node_name = item->text();
            if (node_name.isEmpty()) {
                qDebug() << warning_prefix
                         << "Project tree model contains a node with an empty "
                            "name. Skipping node";
                continue;
            }
            if (current.sibling_names.contains(node_name)) {
                qDebug() << warning_prefix
                         << "Json project tree contains a node with duplicate "
                            "sibling names. Skipping node";
                continue;
            }
            current.sibling_names.insert(node_name);

            QJsonObject node_object;
            node_object.insert(NAME_KEY, node_name);

            const QString node_path = item->data(PATH_ROLE).toString();
            if (!node_path.isEmpty()) {
                node_object.insert(PATH_KEY, node_path);
                current.child_nodes.append(node_object);
                continue;
            }

            // Virtual node, written straight away when it is a leaf.
            if (item->rowCount() == 0) {
                node_object.insert(VIRTUAL_NODES_KEY, QJsonArray());
                current.child_nodes.append(node_object);
                continue;
            }

            // Otherwise save the node on the level and collect its children
            // in the level pushed for it.
            current.node = node_object;
            pending.push_back({item, 0, {}, {}, node_object});
        }

        return std::make_unique<QJsonArray>(root_nodes).release();
    }
    // bool create_project_tree_json_array_file_from_model(
    //     const QStandardItemModel &model, QFile &file) {
    //     auto project_tree_json_array_opt =
    //         create_project_tree_json_array_from_model(model);
    //     if (!project_tree_json_array_opt.has_value())
    //         qWarning() << "Failed to save project tree json file: could not "
    //                       "create project tree json array from model";
    //
    //     QJsonArray *project_tree_json_array =
    //         project_tree_json_array_opt.value();
    //     const QJsonDocument

} // namespace ProjectTreeModel
} // namespace OpenConfigEditor
