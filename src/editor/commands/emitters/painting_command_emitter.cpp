#include <core/os/input.h>
#include <modules/isometric_maps/src/utils/isometric_maths.h>
#include <modules/isometric_maps/src/isometric_server.h>
#include <modules/isometric_maps/src/editor/isometric_editor_plugin.h>
#include "painting_command_emitter.h"

using namespace editor::commands::emitters;

Vector<Ref<editor::commands::AddPositionableCommand>>
PaintingCommandEmitter::from_gui_input_to_command_impl(Ref<InputEventMouseMotion> p_event) { // NOLINT(performance-unnecessary-value-param)
    Vector<Ref<editor::commands::AddPositionableCommand>> commands;
    if (!Input::get_singleton()->is_mouse_button_pressed(BUTTON_LEFT)) {
        if (was_last_event_intercepted) {
            was_last_event_intercepted = false;
        }
        return commands;
    }

    const data::IsometricParameters* parameters{
        IsometricServer::get_instance()->get_space_configuration(map->get_space_RID())
    };

    const Vector3& position{
            utils::from_screen_to_3D(
                    *parameters,
                    map->get_local_mouse_position(),
                    0
            )
    };

    if (was_last_event_intercepted && last_event_position == position) {
        return commands;
    }

    if (!was_last_event_intercepted) {
        was_last_event_intercepted = true;
    }

    int selected_tile_id{
        editor::IsometricEditorPlugin::get_instance()->get_selection_pane()->get_selected_positionable_id()
    };

    if (selected_tile_id == resource::PositionableSet::NONE_VALUE) {
        return commands;
    }

    if (!map->is_position_in_map(position)) {
        return commands;
    }

    if (map->get_positionable_id_at(position) != resource::PositionableSet::NONE_VALUE) {
        return commands;
    }

    last_event_position = position;

    Ref<editor::commands::AddPositionableCommand> add_command;
    add_command.instance();
    add_command->set_map(map);
    add_command->set_position(position);
    add_command->set_positionable_id(selected_tile_id);
    commands.push_back(add_command);
    print_line(position);
    return commands;
}

void PaintingCommandEmitter::set_map(node::IsometricMap* p_map) {
    map = p_map;
}

PaintingCommandEmitter::PaintingCommandEmitter(UndoRedo* undo_redo) : CommandEmitter(undo_redo), map(nullptr),
                                                                      last_event_position(), was_last_event_intercepted(false) {

}
