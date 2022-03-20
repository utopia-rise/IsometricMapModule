#ifdef TOOLS_ENABLED

#include <modules/isometric_maps/src/editor/isometric_editor_plugin.h>
#include "move_editor_plane_command.h"

using namespace editor::commands;

void MoveEditorPlaneCommand::redo() {
    act(is_forward);
}

void MoveEditorPlaneCommand::undo() {
    act(!is_forward);
}

void MoveEditorPlaneCommand::act(bool p_is_forward) {
    IsometricEditorPlugin* isometric_editor_plugin = IsometricEditorPlugin::get_instance();
    EditorPlane& editor_plane{isometric_editor_plugin->get_editor_plane_for_selected_map()};
    int new_position{editor_plane.get_position()};
    if (p_is_forward) {
        new_position += 1;
    } else {
        new_position -= 1;
    }

    editor_plane.set_position(new_position);
    isometric_editor_plugin->refresh();
}

void MoveEditorPlaneCommand::set_is_forward(bool p_is_forward) {
    is_forward = p_is_forward;
}

MoveEditorPlaneCommand::MoveEditorPlaneCommand() :
    Command(),
    is_forward(false) {

}

#endif