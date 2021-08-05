#include <core/os/input.h>
#include <modules/isometric_maps/src/utils/isometric_maths.h>
#include <modules/isometric_maps/src/isometric_server.h>
#include "painting_command_emitter.h"

using namespace editor::commands::emitters;

Vector<Ref<editor::commands::AddPositionableCommand>> PaintingCommandEmitter::from_gui_input_to_command_impl(
        const InputEventMouseMotion* p_event) {
    Vector<Ref<editor::commands::AddPositionableCommand>> commands;
    if (!Input::get_singleton()->is_mouse_button_pressed(BUTTON_LEFT)) {
        return commands;
    }
    print_line("Event forwarded to PaintingCommandEmitter");
    Ref<editor::commands::AddPositionableCommand> add_command;
    add_command.instance();
    add_command->set_map(map);
    const data::IsometricParameters* parameters{
        IsometricServer::get_instance()->get_space_configuration(map->get_space_RID())
    };
    const Vector3& position{
            utils::from_screen_to_3D(*parameters, map->to_local(p_event->get_global_position()), 0)
    };
    print_line(position);
    add_command->set_position(position);
//    commands.push_back(add_command);
    return commands;
}

void PaintingCommandEmitter::set_map(node::IsometricMap* p_map) {
    map = p_map;
}

PaintingCommandEmitter::PaintingCommandEmitter(UndoRedo* undo_redo) : CommandEmitter(undo_redo), map(nullptr) {

}
