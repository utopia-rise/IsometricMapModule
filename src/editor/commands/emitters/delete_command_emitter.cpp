#ifdef TOOLS_ENABLED

#include "delete_command_emitter.h"
#include <core/os/keyboard.h>
#include <modules/isometric_maps/src/editor/positionable_selector_manager.h>
#include <modules/isometric_maps/src/editor/commands/revert_command.h>
#include <modules/isometric_maps/src/editor/commands/add_positionable_command.h>
#include <modules/isometric_maps/src/editor/commands/select_positionable_command.h>
#include <modules/isometric_maps/src/editor/commands/composite_command.h>

using namespace editor::commands::emitters;

Vector<Ref<editor::commands::Command>>
DeleteCommandEmitter::from_gui_input_to_command_impl(Ref<InputEventKey> p_event) { // NOLINT(performance-unnecessary-value-param)
    Vector<Ref<editor::commands::Command>> commands;

    if (!p_event->is_pressed()) {
        return commands;
    }

    if (p_event->get_scancode() != KeyList::KEY_BACKSPACE) {
        return commands;
    }

    const Vector<Vector3>& selected_positions{
        editor::PositionableSelectorManager::get_instance().get_selected_for_map(map)
    };

    for (int i = 0; i < selected_positions.size(); ++i) {
        const Vector3& position{selected_positions[i]};
        if (node::IsometricPositionable* current{map->get_positionable_at(position)}) {
            const Vector3& local_position{current->get_local_position_3d()};

            Ref<SelectPositionableCommand> select_command;
            select_command.instance();
            select_command->set_should_deselect_first(false);
            select_command->set_map(map);
            select_command->set_position(position);

            Ref<RevertCommand> deselect_command;
            deselect_command.instance();
            deselect_command->set_reverse_command(select_command);

            Ref<AddPositionableCommand> add_command;
            add_command.instance();
            add_command->set_map(map);
            add_command->set_aabb({local_position, current->get_size()});
            add_command->set_positionable_id(map->get_positionable_id_for_position(local_position));

            Ref<RevertCommand> delete_command;
            delete_command.instance();
            delete_command->set_reverse_command(add_command);

            Ref<CompositeCommand> composite_command;
            composite_command.instance();
            composite_command->append_command(deselect_command);
            composite_command->append_command(delete_command);

            commands.push_back(composite_command);
        }
    }

    return commands;
}

void DeleteCommandEmitter::set_map(node::IsometricMap* p_map) {
    map = p_map;
}

DeleteCommandEmitter::DeleteCommandEmitter(UndoRedo* undo_redo) : CommandEmitter(undo_redo), map(nullptr) {

}

#endif