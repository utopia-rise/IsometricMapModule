#ifdef TOOLS_ENABLED
#include "delete_command_emitter.h"

#include "editor/commands/add_positionable_command.h"
#include "editor/commands/composite_command.h"
#include "editor/commands/revert_command.h"
#include "editor/commands/select_positionable_command.h"
#include "editor/isometric_editor_plugin.h"
#include "editor/positionable_selector_manager.h"

#include <core/os/keyboard.h>

using namespace editor::commands::emitters;

Vector<Ref<editor::commands::Command<node::IsometricMap>>> DeleteCommandEmitter::from_gui_input_to_command_impl(Ref<InputEventKey> p_event) {
    Vector<Ref<editor::commands::Command<node::IsometricMap>>> commands;

    if (!p_event->is_pressed()) { return commands; }

    if (p_event->get_keycode() != Key::BACKSPACE) { return commands; }

    node::IsometricMap* map {IsometricEditorPlugin::get_instance()->get_selected_map()};

    const Vector<Vector3>& selected_positions {editor::PositionableSelectorManager::get_instance().get_selected_for_map(map)};

    for (int i = 0; i < selected_positions.size(); ++i) {
        const Vector3& position {selected_positions[i]};
        if (node::IsometricPositionable* current {map->get_positionable_at(position)}) {
            const Vector3& local_position {current->get_local_position_3d()};

            Ref<SelectPositionableCommand> select_command;
            select_command.instantiate();
            select_command->set_should_deselect_first(false);
            select_command->set_position(position);

            Ref<RevertCommand<node::IsometricMap>> deselect_command;
            deselect_command.instantiate();
            deselect_command->set_reverse_command(select_command);

            Ref<AddPositionableCommand> add_command;
            add_command.instantiate();
            add_command->set_layer_id(map->get_layer_id_at(position));
            add_command->set_aabb({local_position, current->get_size()});
            add_command->set_positionable_id(map->get_positionable_id_for_position(local_position));

            Ref<RevertCommand<node::IsometricMap>> delete_command;
            delete_command.instantiate();
            delete_command->set_reverse_command(add_command);

            Ref<CompositeCommand<node::IsometricMap>> composite_command;
            composite_command.instantiate();
            composite_command->append_command(deselect_command);
            composite_command->append_command(delete_command);

            commands.push_back(composite_command);
        }
    }

    return commands;
}

#endif