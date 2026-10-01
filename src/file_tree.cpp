#include "file_tree.h"

namespace OpenConfigEditor {
namespace ProjectTreeModel {

    std::expected<QAbstractItemModel *, ModelCreationError>
    create_model_from_project_tree_json(const QJsonObject &project_tree_json) {
        // recursively read project tree json
        return std::unexpected(ModelCreationError::NAME_AND_DIRECTORY_MISSING);
    }

} // namespace ProjectTreeModel
} // namespace OpenConfigEditor
