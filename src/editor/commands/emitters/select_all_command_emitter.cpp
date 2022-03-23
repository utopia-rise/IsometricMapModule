#ifdef TOOLS_ENABLED

#include <core/os/keyboard.h>
#include <modules/isometric_maps/src/editor/commands/select_positionable_command.h>
#include <modules/isometric_maps/src/editor/positionable_selector_manager.h>
#include <modules/isometric_maps/src/editor/isometric_editor_plugin.h>
#include "select_all_command_emitter.h"

using namespace editor::commands::emitters;

Vector<Ref<editor::commands::Command>>
SelectAllCommandEmitter::from_gui_input_to_command_impl(Ref<InputEventKey> p_event) {
    Vector<Ref<Command>> commands;

    if (!p_event->is_pressed()
        ||p_event->get_scancode() != KeyList::KEY_A
        || !(p_event->get_control() || p_event->get_command())) {
        return commands;
    }

    node::IsometricMap* map{IsometricEditorPlugin::get_instance()->get_selected_map()};

    for (int i = 0; i < map->get_child_count(); ++i) {
        if (node::IsometricPositionable* positionable{Object::cast_to<node::IsometricPositionable>(map->get_child(i))}) {
            if (editor::PositionableSelectorManager::get_instance().is_position_selected_for_map(
                    map, positionable->get_local_position_3d())) {
                continue;
            }
            Ref<editor::commands::SelectPositionableCommand> command;
            command.instance();
            command->set_position(positionable->get_local_position_3d());
            command->set_should_deselect_first(false);
            commands.push_back(command);
        }
    }

    return commands;
}

SelectAllCommandEmitter::SelectAllCommandEmitter(UndoRedo* undo_redo) :
    CommandEmitter<SelectAllCommandEmitter, InputEventKey>(undo_redo) {

}

#endif
