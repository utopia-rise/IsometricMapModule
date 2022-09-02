#ifdef TOOLS_ENABLED

#include <core/os/keyboard.h>
#include <modules/isometric_maps/src/editor/commands/move_editor_plane_command.h>
#include <modules/isometric_maps/src/editor/isometric_editor_plugin.h>
#include "move_editor_plane_command_emitter.h"

using namespace editor::commands::emitters;

Vector<Ref<editor::commands::Command>>
MoveEditorPlaneCommandEmitter::from_gui_input_to_command_impl(Ref<InputEventKey> p_event) {
    Vector<Ref<editor::commands::Command>> commands;

    if (!p_event->is_pressed()) {
        return commands;
    }

    Ref<editor::commands::MoveEditorPlaneCommand> move_command;
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
    EditorPlane& editor_plane{isometric_editor_plugin->get_editor_plane_for_selected_map()};
    real_t editor_plane_position{static_cast<real_t>(editor_plane.get_position())};
    switch (editor_plane.get_axis()) {
        case Vector3::AXIS_X:
            if ((editor_plane_position == 0 && !is_forward) || (editor_plane_position == map->get_size().x - 1 && is_forward)) {
                return commands;
            }
            break;
        case Vector3::AXIS_Y:
            if ((editor_plane_position == 0 && !is_forward) || (editor_plane_position == map->get_size().y - 1 && is_forward)) {
                return commands;
            }
            break;
        case Vector3::AXIS_Z:
            if ((editor_plane_position == 0 && !is_forward) || (editor_plane_position == map->get_size().z - 1 && is_forward)) {
                return commands;
            }
            break;
        default:
            break;
    }

    move_command.instance();
    move_command->set_is_forward(is_forward);
    commands.push_back(move_command);

    return commands;
}

MoveEditorPlaneCommandEmitter::MoveEditorPlaneCommandEmitter(UndoRedo* undo_redo) : CommandEmitter(undo_redo) {

}

#endif
