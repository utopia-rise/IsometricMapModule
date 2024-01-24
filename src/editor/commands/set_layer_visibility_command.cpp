#ifdef TOOLS_ENABLED

#include "set_layer_visibility_command.h"
#include <editor/isometric_editor_plugin.h>

using namespace editor::commands;

void SetLayerVisibilityCommand::redo() {
    IsometricEditorPlugin::get_instance()->get_selected_map()->set_layer_visible(layer_id, is_visible);
}

void SetLayerVisibilityCommand::undo() {
    IsometricEditorPlugin::get_instance()->get_selected_map()->set_layer_visible(layer_id, !is_visible);
}

void SetLayerVisibilityCommand::set_layer_id(uint32_t p_layer_id) {
    layer_id = p_layer_id;
}

void SetLayerVisibilityCommand::set_visible(bool p_is_visible) {
    is_visible = p_is_visible;
}

SetLayerVisibilityCommand::SetLayerVisibilityCommand() : layer_id(node::IsometricMap::DEFAULT_LAYER_ID), is_visible() {

}

#endif