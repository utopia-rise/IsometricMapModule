#ifdef TOOLS_ENABLED

#include <modules/isometric_maps/src/data/isometric_parameters.h>
#include <modules/isometric_maps/src/isometric_server.h>
#include <modules/isometric_maps/src/utils/isometric_maths.h>
#include "select_command_emitter.h"

using namespace editor::commands::emitters;

Vector<Ref<editor::commands::Command>>
SelectCommandEmitter::from_gui_input_to_command_impl(Ref<InputEventMouse> p_event) { // NOLINT(performance-unnecessary-value-param)
    Vector<Ref<Command>> commands;

    if (!p_event->is_pressed()) {
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

    if (!map->get_positionable_at(position)) {
        return commands;
    }

    Ref<editor::commands::SelectPositionableCommand> command;
    command.instance();
    command->set_map(map);
    command->set_position(position);
    command->set_should_deselect_first(!p_event->get_control());

    commands.push_back(command);

    return commands;
}

void SelectCommandEmitter::set_map(node::IsometricMap* p_map) {
    map = p_map;
}

SelectCommandEmitter::SelectCommandEmitter(UndoRedo* undo_redo) :
        CommandEmitter<SelectCommandEmitter, InputEventMouse>(undo_redo),
        map(nullptr) {

}

#endif