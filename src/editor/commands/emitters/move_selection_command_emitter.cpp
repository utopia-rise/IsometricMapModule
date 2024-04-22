#include "move_selection_command_emitter.h"

#include "editor/commands/revert_command.h"
#include "editor/isometric_editor_plugin.h"
#include "editor/positionable_selector_manager.h"
#include "isometric_server.h"
#include "utils/isometric_maths.h"

using namespace editor::commands::emitters;

static Color CONFLICT_MODULATE_COLOR {1, 0, 0, 0.5};
static Color DEFAULT_MODULATE_COLOR {1, 1, 1, 0.5};

Vector<Ref<editor::commands::Command<node::IsometricMap>>> MoveSelectionCommandEmitter::from_gui_input_to_command_impl([[maybe_unused]] Ref<InputEventMouse> p_event) {
    Vector<Ref<editor::commands::Command<node::IsometricMap>>> commands;

    node::IsometricMap* map {IsometricEditorPlugin::get_instance()->get_selected_map()};

    bool is_activated {initial_mouse_position != Vector3(-1, -1, -1)};

    if (p_event->get_button_mask().has_flag(MouseButtonMask::RIGHT) || (!p_event->is_shift_pressed() && !is_activated)) {
        for (const KeyValue<Vector3, node::IsometricPositionable*>& keyValuePair : current_preview_nodes) {
            node::IsometricPositionable* positionable {keyValuePair.value};
            map->remove_child(positionable);
            memdelete(positionable);

            map->get_positionable_at(keyValuePair.key)->set_visible(true);
        }
        current_preview_nodes.clear();

        initial_mouse_position = Vector3(-1, -1, -1);
        new_mouse_position = Vector3(-1, -1, -1);

        is_move_valid = true;

        return commands;
    }

    const data::IsometricParameters* parameters {
      IsometricServer::get_instance()->space_get_configuration(map->get_space_RID())
    };
    EditorPlane& editor_plane {
      IsometricEditorPlugin::get_instance()->get_editor_plane_for_map(
        map,
        EditorPlane::PlaneType::EDITOR_DRAWER
      )
    };

    Vector3 mouse_position {
      utils::from_screen_to_3D(
        *parameters,
        map->get_local_mouse_position(),
        editor_plane.get_axis(),
        static_cast<float>(editor_plane.get_position())
      )
    };

    if (Input::get_singleton()->is_mouse_button_pressed(MouseButton::LEFT)) {
        is_move_valid = true;

        if (!is_activated) {
            initial_mouse_position = mouse_position;

            const Vector<Vector3>& selected_positions { PositionableSelectorManager::get_instance().get_selected_for_map(map) };

            if (selected_positions.is_empty()) return commands;

            for (const Vector3& position : selected_positions) {
                node::IsometricPositionable* original_positionable = map->get_positionable_at(position);
                auto* duplicate {
                  Object::cast_to<node::IsometricPositionable>(original_positionable->duplicate()
                  )
                };
                original_positionable->set_visible(false);
                duplicate->set_is_dynamic(true);

                map->add_child(duplicate);

                current_preview_nodes[position] = duplicate;
                initial_layer_ids[position] = map->get_layer_id_at(position);
            }
        }

        new_mouse_position = mouse_position;

        Vector3 position_delta { new_mouse_position - initial_mouse_position };

        for (const KeyValue<Vector3, node::IsometricPositionable*>& keyValuePair : current_preview_nodes) {
            node::IsometricPositionable* positionable {keyValuePair.value};
            Vector3 new_position { keyValuePair.key + position_delta };

            if ((map->get_positionable_at(new_position) && !current_preview_nodes.has(new_position)) ||
                !map->is_aabb_in_map({new_position, positionable->get_size()})) {
                positionable->set_editor_modulate(CONFLICT_MODULATE_COLOR);
                is_move_valid = false;
            } else {
                positionable->set_editor_modulate(DEFAULT_MODULATE_COLOR);
            }

            positionable->set_local_position_3d(new_position);
        }

    } else if (is_activated && is_move_valid) {
        for (const KeyValue<Vector3, node::IsometricPositionable*>& keyValuePair : current_preview_nodes) {
            Vector3 initial_position { keyValuePair.key };
            node::IsometricPositionable* positionable {keyValuePair.value};

            Ref<editor::commands::SelectPositionableCommand> select_command;
            select_command.instantiate();
            select_command->set_position(initial_position);
            Ref<editor::commands::RevertCommand<node::IsometricMap>> deselect_command;
            deselect_command.instantiate();
            deselect_command->set_reverse_command(select_command);
            commands.push_back(deselect_command);

            Ref<editor::commands::AddPositionableCommand> add_command;
            add_command.instantiate();
            Vector3 positionable_size { positionable->get_size() };
            add_command->set_aabb({initial_position, positionable_size});
            add_command->set_positionable_id(map->get_positionable_id_for_position(initial_position));
            add_command->set_layer_id(initial_layer_ids[initial_position]);
            Ref<editor::commands::RevertCommand<node::IsometricMap>> remove_command;
            remove_command.instantiate();
            remove_command->set_reverse_command(add_command);
            commands.push_back(remove_command);
        }
        
        for (const KeyValue<Vector3, node::IsometricPositionable*>& keyValuePair : current_preview_nodes) {
            Vector3 initial_position { keyValuePair.key };
            node::IsometricPositionable* positionable {keyValuePair.value};
            
            Vector3 new_position {positionable->get_local_position_3d()};
            Vector3 positionable_size { positionable->get_size() };

            Ref<editor::commands::AddPositionableCommand> add_command;
            add_command.instantiate();
            add_command->set_aabb({new_position, positionable_size});
            add_command->set_positionable_id(map->get_positionable_id_for_position(initial_position));
            add_command->set_layer_id(initial_layer_ids[initial_position]);
            commands.push_back(add_command);
            
            Ref<editor::commands::SelectPositionableCommand> select_command;
            select_command.instantiate();
            select_command->set_position(new_position);
            commands.push_back(select_command);

            map->remove_child(positionable);
            memdelete(positionable);
        }
        current_preview_nodes.clear();
        
        initial_mouse_position = Vector3(-1, -1, -1);
        new_mouse_position = Vector3(-1, -1, -1);
    }

    return commands;
}