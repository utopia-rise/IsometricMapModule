#ifdef TOOLS_ENABLED

#include <modules/isometric_maps/src/editor/isometric_editor_plugin.h>
#include "rotate_editor_plane_command.h"

using namespace editor::commands;

void RotateEditorPlaneCommand::redo() {
    set_axis_and_position(new_axis, 0);
}

void RotateEditorPlaneCommand::undo() {
    set_axis_and_position(former_axis, former_position);
}

void RotateEditorPlaneCommand::set_new_axis(Vector3::Axis p_axis) {
    new_axis = p_axis;
}

void RotateEditorPlaneCommand::set_former_axis(Vector3::Axis p_axis) {
    former_axis = p_axis;
}

void RotateEditorPlaneCommand::set_former_position(int p_position) {
    former_position = p_position;
}

void RotateEditorPlaneCommand::set_axis_and_position(Vector3::Axis p_axis, int p_position) {
    IsometricEditorPlugin* isometric_editor_plugin{IsometricEditorPlugin::get_instance()};
    EditorPlane& editor_plane{isometric_editor_plugin->get_editor_plane_for_selected_map()};

    editor_plane.set_position(p_position);
    editor_plane.set_axis(p_axis);

    isometric_editor_plugin->refresh();
}

#endif