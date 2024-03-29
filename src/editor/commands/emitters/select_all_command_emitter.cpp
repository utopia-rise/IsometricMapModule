#ifdef TOOLS_ENABLED
#include "select_all_command_emitter.h"

#include "editor/commands/select_positionable_command.h"
#include "editor/isometric_editor_plugin.h"
#include "editor/positionable_selector_manager.h"

using namespace editor::commands::emitters;

Vector<Ref<editor::commands::Command<node::IsometricMap>>> SelectAllCommandEmitter::from_gui_input_to_command_impl(Ref<InputEventKey> p_event) {
    Vector<Ref<Command<node::IsometricMap>>> commands;

    if (!p_event->is_pressed() || p_event->get_keycode() != Key::A || !p_event->is_command_or_control_pressed()) {
        return commands;
    }

    IsometricEditorPlugin* isometric_editor_plugin {IsometricEditorPlugin::get_instance()};
    node::IsometricMap* map {IsometricEditorPlugin::get_instance()->get_selected_map()};

    Vector<node::IsometricPositionable*> positionables {map->get_positionables_in(
      {{static_cast<real_t>(
          isometric_editor_plugin->get_editor_plane_for_map(map, EditorPlane::PlaneType::X_MIN_VIEW_LIMITER).get_position()
        ),
        static_cast<real_t>(
          isometric_editor_plugin->get_editor_plane_for_map(map, EditorPlane::PlaneType::Y_MIN_VIEW_LIMITER).get_position()
        ),
        static_cast<real_t>(
          isometric_editor_plugin->get_editor_plane_for_map(map, EditorPlane::PlaneType::Z_MIN_VIEW_LIMITER).get_position()
        )},
       {static_cast<real_t>(
          isometric_editor_plugin->get_editor_plane_for_map(map, EditorPlane::PlaneType::X_MAX_VIEW_LIMITER).get_position()
        ),
        static_cast<real_t>(
          isometric_editor_plugin->get_editor_plane_for_map(map, EditorPlane::PlaneType::Y_MAX_VIEW_LIMITER).get_position()
        ),
        static_cast<real_t>(
          isometric_editor_plugin->get_editor_plane_for_map(map, EditorPlane::PlaneType::Z_MAX_VIEW_LIMITER).get_position()
        )}}
    )};

    for (int i = 0; i < positionables.size(); ++i) {
        if (node::IsometricPositionable * positionable {positionables[i]}) {
            if (editor::PositionableSelectorManager::get_instance().is_position_selected_for_map(map, positionable->get_local_position_3d())) {
                continue;
            }
            Ref<editor::commands::SelectPositionableCommand> command;
            command.instantiate();
            command->set_position(positionable->get_local_position_3d());
            command->set_should_deselect_first(false);
            commands.push_back(command);
        }
    }

    return commands;
}

#endif
