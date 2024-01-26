#ifdef TOOLS_ENABLED
#include "select_command_emitter.h"

#include "data/isometric_parameters.h"
#include "editor/isometric_editor_plugin.h"
#include "isometric_server.h"
#include "utils/isometric_maths.h"

using namespace editor::commands::emitters;

Vector<Ref<editor::commands::Command<node::IsometricMap>>> SelectCommandEmitter::from_gui_input_to_command_impl(Ref<InputEventMouse> p_event) {// NOLINT(performance-unnecessary-value-param)
    Vector<Ref<Command<node::IsometricMap>>> commands;

    if (!p_event->is_pressed()) { return commands; }

    IsometricEditorPlugin* isometric_editor_plugin {IsometricEditorPlugin::get_instance()};
    node::IsometricMap* map {isometric_editor_plugin->get_selected_map()};

    const data::IsometricParameters* parameters {IsometricServer::get_instance()->space_get_configuration(map->get_space_RID())};

    EditorPlane& editor_plane = isometric_editor_plugin->get_editor_plane_for_map(map, EditorPlane::PlaneType::EDITOR_DRAWER);
    const Vector3& position {utils::from_screen_to_3D(
      *parameters,
      map->get_local_mouse_position(),
      editor_plane.get_axis(),
      static_cast<float>(editor_plane.get_position())
    )};

    if (!isometric_editor_plugin->is_aabb_in_view_limiters({position, {1, 1, 1}})) { return commands; }

    if (!map->get_positionable_at(position)) { return commands; }

    Ref<editor::commands::SelectPositionableCommand> command;
    command.instantiate();
    command->set_position(position);
    command->set_should_deselect_first(!p_event->is_command_or_control_pressed());

    commands.push_back(command);

    return commands;
}

#endif