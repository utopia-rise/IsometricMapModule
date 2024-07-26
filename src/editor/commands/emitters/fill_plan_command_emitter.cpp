#ifdef TOOLS_ENABLED

#include "fill_plan_command_emitter.h"

#include "editor/isometric_editor_plugin.h"
#include "isometric_server.h"
#include "utils/isometric_maths.h"

using namespace editor::commands::emitters;

static constexpr bool (*condition)(node::IsometricPositionable*) = [](node::IsometricPositionable* positionable) -> bool {
    return positionable == nullptr;
};

Vector<Ref<editor::commands::Command<node::IsometricMap>>> FillPlanCommandEmitter::from_gui_input_to_command_impl(Ref<InputEventMouse> p_event) {
    Vector<Ref<Command<node::IsometricMap>>> commands;

    if (!Input::get_singleton()->is_mouse_button_pressed(MouseButton::LEFT)) {
        return commands;
    }

    IsometricEditorPlugin* isometric_editor_plugin { IsometricEditorPlugin::get_instance() };

    int selected_tile_id {isometric_editor_plugin->get_selection_pane()->get_selected_positionable_id()};

    if (selected_tile_id == resource::PositionableSet::NONE_POSITIONABLE_ID) { return commands; }

    node::IsometricMap* map { isometric_editor_plugin->get_selected_map() };
    const EditorPlane& editor_plane {
      isometric_editor_plugin->get_editor_plane_for_map(map, EditorPlane::PlaneType::EDITOR_DRAWER)
    };

    Vector3::Axis editor_plane_axis { editor_plane.get_axis() };

    const data::IsometricParameters* parameters {IsometricServer::get_instance()->space_get_configuration(map->get_space_RID())};

    const Vector3& position {
      utils::from_screen_to_3D(
        *parameters,
        map->get_local_mouse_position(),
        editor_plane_axis,
        static_cast<float>(editor_plane.get_position())
      )
    };

    Vector3 positionable_size;
    if (auto* positionable {Object::cast_to<node::IsometricPositionable>(
          map->get_positionable_set()->get_positionable_scene_for_id(selected_tile_id)->instantiate()
        )}) {
        positionable_size = positionable->get_size();
        memdelete(positionable);
    } else {
        return commands;
    }

    Vector<Vector3> connected_positions_with_no_elements { map->dfs<condition>(position, editor_plane_axis, positionable_size) };

    for (const Vector3& connected_position : connected_positions_with_no_elements) {
        Ref<editor::commands::AddPositionableCommand> add_command;
        add_command.instantiate();
        add_command->set_aabb({connected_position, positionable_size});
        add_command->set_positionable_id(selected_tile_id);
        add_command->set_layer_id(IsometricEditorPlugin::get_instance()->get_selected_layer());
        commands.push_back(add_command);
    }

    return commands;
}


#endif