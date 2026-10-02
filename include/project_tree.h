#pragma once

#include "qdir.h"
#include "qjsondocument.h"
#include "qstandarditemmodel.h"
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QModelIndex>
#include <QString>
#include <QTreeView>

#include <optional>

namespace OpenConfigEditor {

namespace ProjectTreeModel {

    // Item data roles holding the canonical filepath of a node that points
    // at something real. Which of the two a node has is what says whether
    // the node is a file or a directory, so a view can tell them apart
    // without asking the filesystem about the path again. A virtual node
    // points at nothing, so it has neither.
    enum Role {
        FILE_PATH_ROLE = Qt::UserRole + 1,
        DIRECTORY_PATH_ROLE = Qt::UserRole + 2,
    };

    extern std::optional<QStandardItemModel *>
    create_model_from_project_tree_json(const QJsonArray &root_array);
    extern std::optional<QStandardItemModel *>
    create_model_from_project_tree_json_file(QFile &project_tree_json);

    extern std::optional<QJsonArray *>
    create_project_tree_json_array_from_model(const QStandardItemModel &model);
    // extern bool save_project_tree_json_array_file_from_model(
    //     const QStandardItemModel &model, QFile &file);
}; // namespace ProjectTreeModel

// A QTreeView over a ProjectTreeModel, which owns the model it shows and
// knows how to go from an index back to the filepath baked into that node.
class ProjectTree : public QTreeView {
    Q_OBJECT

  public:
    ProjectTree(QWidget *parent = nullptr);
    ~ProjectTree() override;

    // Rebuilds the model from a project tree json file, destroying the
    // previous one. The current model is left in place when the file
    // cannot be read or parsed.
    //
    // @return true when the model was replaced
    bool load_project_tree_json_file(const QString &project_tree_json_path);

    // Info about the filepath of the node at index, if that node has one
    // baked in under role. Which role is asked for is what says whether the
    // node is a file or a directory, so asking for FILE_PATH_ROLE on a
    // directory yields nothing rather than a directory. An invalid index,
    // or a node with nothing under that role, yields nothing as well.
    //
    // QFileInfo rather than QFile, since Qt 6 gives QFile neither a copy nor
    // a move constructor and so cannot be held by value, and since isDir on
    // it is what tells a caller whether it has a directory or a file. Take
    // the path off it with filePath, fileName on a QFileInfo is the bare
    // basename and drops the directories above it.
    [[nodiscard]] std::optional<QFileInfo>
    get_file_for_index(const QModelIndex &index, int role) const;

    // Shorthand for the file and directory roles respectively
    [[nodiscard]] std::optional<QFileInfo>
    get_file(const QModelIndex &index) const;
    [[nodiscard]] std::optional<QFileInfo>
    get_directory(const QModelIndex &index) const;

  private:
    // Not owned by the view, so it has to be destroyed by hand, and taken
    // off the view first so nothing reaches for it while it goes away.
    QStandardItemModel *project_tree_model = nullptr;
};

} // namespace OpenConfigEditor
