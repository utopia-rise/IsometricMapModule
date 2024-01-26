#ifdef TOOLS_ENABLED
#include "painting_command_emitter.h"

#include "editor/isometric_editor_plugin.h"
#include "isometric_server.h"
#include "utils/isometric_maths.h"

using namespace editor::commands::emitters;

Vector<Ref<editor::commands::Command<node::IsometricMap>>> PaintingCommandEmitter::from_gui_input_to_command_impl([[maybe_unused]] Ref<InputEventMouse> p_event
) {// NOLINT(performance-unnecessary-value-param)
    Vector<Ref<Command<node::IsometricMap>>> commands;

    _clear_current_preview_node();

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

    int selected_tile_id {isometric_editor_plugin->get_selection_pane()->get_selected_positionable_id()};

    if (selected_tile_id == resource::PositionableSet::NONE_POSITIONABLE_ID) { return commands; }

    Vector3 size;
    if (auto* positionable {Object::cast_to<node::IsometricPositionable>(
          map->get_positionable_set()->get_positionable_scene_for_id(selected_tile_id)->instantiate()
        )}) {
        size = positionable->get_size();
        memdelete(positionable);
    } else {
        return commands;
    }

    AABB aabb {position, size};

    if (!map->is_aabb_in_map(aabb)) { return commands; }

    if (!isometric_editor_plugin->is_aabb_in_view_limiters(aabb)) { return commands; }

    if (map->is_overlapping(aabb)) { return commands; }

    if (!Input::get_singleton()->is_mouse_button_pressed(MouseButton::LEFT)) {
        current_preview_node = Object::cast_to<node::IsometricPositionable>(
          map->get_positionable_set()->get_positionable_scene_for_id(selected_tile_id)->instantiate()
        );

        // TODO: Use the future container node to avoid flickering.
        current_preview_node->set_modulate(Color(1, 1, 1, 0.5));
        current_preview_node->set_is_dynamic(true);
        current_preview_node->set_local_position_3d(position);
        map->add_child(current_preview_node);

        return commands;
    }

    Ref<editor::commands::AddPositionableCommand> add_command;
    add_command.instantiate();
    add_command->set_aabb(aabb);
    add_command->set_positionable_id(selected_tile_id);
    add_command->set_layer_id(IsometricEditorPlugin::get_instance()->get_selected_layer());
    commands.push_back(add_command);
    return commands;
}

void PaintingCommandEmitter::_clear_current_preview_node() {
    if (current_preview_node) {
        node::IsometricMap* map {IsometricEditorPlugin::get_instance()->get_selected_map()};
        map->remove_child(current_preview_node);
        memdelete(current_preview_node);
        current_preview_node = nullptr;
    }
}

PaintingCommandEmitter::PaintingCommandEmitter() :
  current_preview_node(nullptr) {}

PaintingCommandEmitter::~PaintingCommandEmitter() {
    _clear_current_preview_node();
}

#endif