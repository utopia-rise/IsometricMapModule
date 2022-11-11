#ifdef TOOLS_ENABLED

    #include "move_editor_plane_command.h"
    #include <modules/isometric_maps/src/editor/isometric_editor_plugin.h>
    #include <modules/isometric_maps/src/logging.h>

using namespace editor::commands;

void MoveEditorPlaneCommand::redo() {
    act(is_forward);
}

void MoveEditorPlaneCommand::undo() {
    act(!is_forward);
}

void MoveEditorPlaneCommand::act(bool p_is_forward) {
    MAPS_ERR_FAIL_COND_MSG(plane_type == EditorPlane::PlaneType::SIZE, "PlaneType should be set on MoveEditorPlaneCommand");
    IsometricEditorPlugin* isometric_editor_plugin = IsometricEditorPlugin::get_instance();
    EditorPlane& editor_plane {isometric_editor_plugin->get_editor_plane_for_selected_map(plane_type)};
    int new_position {editor_plane.get_position()};
    if (p_is_forward) {
        new_position += 1;
    } else {
        new_position -= 1;
    }

    editor_plane.set_position(new_position);
    isometric_editor_plugin->refresh(plane_type);
}

void MoveEditorPlaneCommand::set_is_forward(bool p_is_forward) {
    is_forward = p_is_forward;
}

void MoveEditorPlaneCommand::set_plane_type(editor::EditorPlane::PlaneType p_plane_type) {
    plane_type = p_plane_type;
}

MoveEditorPlaneCommand::MoveEditorPlaneCommand() : Command(), is_forward(false), plane_type(EditorPlane::PlaneType::SIZE) {}

#endif