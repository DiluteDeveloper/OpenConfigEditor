#include "file_tree.h"

namespace OpenConfigEditor {
namespace ProjectTreeModel {

    std::expected<QAbstractItemModel *, ProjectTreeModelError>
    create_model_from_project_tree_json(const QJsonObject &project_tree_json) {
        // Read a project tree json object. Do not use a recursive
        // function.
        // Here is how the json is formatted:
        // The root object has an array of subobjects,
        // each subobject acts as a node in a filetree,
        // and can have a name, directory, and subtrees.
        // Essentially, when a node has a directory,
        // that directory is going to be a path to
        // a physical location on disk, and the subdirectories
        // will be lazy loaded to mimic that physical location,
        // the JSON cannot have subnodes when node has a directory
        // aka a physical location, subnodes are for virtual nodes.
        // With this method, the node can have a name.
        // If there is no directory, then it is simply a virtual
        // node, it can have a name for the node and can have subnodes
        // that have names and other stuff themselves.
        // This function needs to read the json parameter input and
        // return a heap allocated QAbstractItemModel object with the
        // parsed project tree JSON. Feel free to create more
        // ProjectTreeModelError enum variants to fit different issues
        // with the JSON.
    }
    std::expected<QJsonObject *, ProjectTreeModelError>
    create_project_tree_json_from_model(const QAbstractItemModel &model) {
        // This function needs to create a json object matching the
        // specification mentioned in the comments in
        // create_model_from_project_tree_json, using the model
        // parameter. This is basically the reverse
    }

} // namespace ProjectTreeModel
} // namespace OpenConfigEditor
