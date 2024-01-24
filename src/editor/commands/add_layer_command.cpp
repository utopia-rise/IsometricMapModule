#ifdef TOOLS_ENABLED

#include "add_layer_command.h"
#include "node/isometric_map.h"
#include "editor/isometric_editor_plugin.h"

using namespace editor::commands;

void AddLayerCommand::redo() {
    IsometricEditorPlugin::get_instance()->get_selected_map()->add_layer(layer_name, layer_id);
}

void AddLayerCommand::undo() {
    if (layer_id == node::IsometricMap::NO_LAYER_ID) {
        IsometricEditorPlugin::get_instance()->get_selected_map()->remove_layer(layer_name);
        return;
    }
    IsometricEditorPlugin::get_instance()->get_selected_map()->remove_layer(layer_id);
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
