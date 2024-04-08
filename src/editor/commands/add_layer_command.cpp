#ifdef TOOLS_ENABLED

#include "add_layer_command.h"
#include "node/isometric_map.h"
#include "editor/isometric_editor_plugin.h"

using namespace editor::commands;

void AddLayerCommand::redo() {
    layer_id = context_node->add_layer(layer_name, layer_id);
    Command<node::IsometricMap>::redo();
}

void AddLayerCommand::undo() {
    if (layer_id == node::IsometricMap::NO_LAYER_ID) {
        context_node->remove_layer(layer_name);
    } else {
        context_node->remove_layer(layer_id);
    }
    Command<node::IsometricMap>::undo();
}

void AddLayerCommand::set_layer_id(uint32_t p_layer_id) {
    layer_id = p_layer_id;
}

void AddLayerCommand::set_layer_name(const String& p_layer_name) {
    layer_name = p_layer_name;
}

AddLayerCommand::AddLayerCommand() : layer_id(node::IsometricMap::NO_LAYER_ID) {

}

#endif
