#ifdef TOOLS_ENABLED

#include <modules/isometric_maps/src/editor/isometric_editor_plugin.h>
#include <modules/isometric_maps/src/logging.h>
#include "move_editor_plane_command.h"

using namespace editor::commands;

void MoveEditorPlaneCommand::redo() {
    MAPS_ERR_FAIL_COND_MSG(plane_type == EditorPlane::PlaneType::SIZE, "PlaneType should be set on MoveEditorPlaneCommand");
    IsometricEditorPlugin* isometric_editor_plugin = IsometricEditorPlugin::get_instance();
    EditorPlane& editor_plane{isometric_editor_plugin->get_editor_plane_for_selected_map(plane_type)};

    const Vector3& current_map_size{isometric_editor_plugin->get_selected_map()->get_size()};
    former_position = editor_plane.get_position();
    int new_position{editor_plane.get_position()};

    real_t max_position;
    switch (editor_plane.get_axis()) {

        case Vector3::AXIS_X:
            max_position = current_map_size.x;
            break;
        case Vector3::AXIS_Y:
            max_position = current_map_size.y;
            break;
        case Vector3::AXIS_Z:
            max_position = current_map_size.z;
            break;
    }

    if (is_forward) {
        if (former_position < max_position) {
            new_position += 1;
        }
        if (new_position >= max_position && should_set_inf_on_max) {
            new_position = INT_MAX;
        }
    } else {
        new_position = CLAMP(former_position, 0, max_position) - 1;
    }

    editor_plane.set_position(new_position);
    isometric_editor_plugin->refresh(plane_type);
}

void MoveEditorPlaneCommand::undo() {
    IsometricEditorPlugin* isometric_editor_plugin = IsometricEditorPlugin::get_instance();
    isometric_editor_plugin->get_editor_plane_for_selected_map(plane_type).set_position(former_position);
    isometric_editor_plugin->refresh(plane_type);
}

void MoveEditorPlaneCommand::set_is_forward(bool p_is_forward) {
    is_forward = p_is_forward;
}

void MoveEditorPlaneCommand::set_plane_type(editor::EditorPlane::PlaneType p_plane_type) {
    plane_type = p_plane_type;
}

void MoveEditorPlaneCommand::set_should_set_inf_on_max(bool p_should_set_inf_on_max) {
    should_set_inf_on_max = p_should_set_inf_on_max;
}

MoveEditorPlaneCommand::MoveEditorPlaneCommand() :
    Command(),
    is_forward(false),
    plane_type(EditorPlane::PlaneType::SIZE),
    should_set_inf_on_max(false),
    former_position() {

}

#endif