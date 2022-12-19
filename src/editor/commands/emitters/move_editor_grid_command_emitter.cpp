#ifdef TOOLS_ENABLED

#include <core/os/keyboard.h>
#include <modules/isometric_maps/src/editor/commands/move_editor_plane_command.h>
#include <modules/isometric_maps/src/editor/isometric_editor_plugin.h>
#include "move_editor_grid_command_emitter.h"

using namespace editor::commands::emitters;

Vector<Ref<editor::commands::Command>>
MoveEditorGridCommandEmitter::from_gui_input_to_command_impl(Ref<InputEventKey> p_event) {
    Vector<Ref<editor::commands::Command>> commands;

    if (!p_event->is_pressed()) {
        return commands;
    }

    bool is_forward{false};
    switch (p_event->get_scancode()) {
        case KeyList::KEY_UP:
            is_forward = true;
            break;
        case KeyList::KEY_DOWN:
            break;
        default:
            return commands;
    }

    IsometricEditorPlugin* isometric_editor_plugin{IsometricEditorPlugin::get_instance()};
    node::IsometricMap* map{isometric_editor_plugin->get_selected_map()};
    EditorPlane& editor_plane{isometric_editor_plugin->get_editor_plane_for_selected_map(EditorPlane::PlaneType::EDITOR_DRAWER)};

    int current_position{editor_plane.get_position()};
    int new_position{};
    switch (editor_plane.get_axis()) {
        case Vector3::AXIS_X:
            if ((current_position == 0 && !is_forward) || (current_position >= map->get_size().x - 1 && is_forward)) {
                return commands;
            }
            new_position = MIN(current_position, map->get_size().x);
            break;
        case Vector3::AXIS_Y:
            if ((current_position == 0 && !is_forward) || (current_position >= map->get_size().y - 1 && is_forward)) {
                return commands;
            }
            new_position = MIN(current_position, map->get_size().y);
            break;
        case Vector3::AXIS_Z:
            if ((current_position == 0 && !is_forward) || (current_position >= map->get_size().z - 1 && is_forward)) {
                return commands;
            }
            new_position = MIN(current_position, map->get_size().z);
            break;
        default:
            break;
    }

    if (is_forward) {
        new_position += 1;
    } else {
        new_position -= 1;
    }

    Ref<editor::commands::MoveEditorPlaneCommand> move_command;
    move_command.instance();
    move_command->set_plane_type(EditorPlane::PlaneType::EDITOR_DRAWER);
    move_command->set_old_position(current_position);
    move_command->set_new_position(new_position);
    commands.push_back(move_command);

    return commands;
}

MoveEditorGridCommandEmitter::MoveEditorGridCommandEmitter(UndoRedo* undo_redo) : CommandEmitter(undo_redo) {

}

#endif
