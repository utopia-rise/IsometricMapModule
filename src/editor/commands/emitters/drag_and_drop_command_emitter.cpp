#ifdef TOOLS_ENABLED
#include "drag_and_drop_command_emitter.h"

#include "data/isometric_parameters.h"
#include "editor/isometric_editor_plugin.h"
#include "isometric_server.h"
#include "utils/isometric_maths.h"

using namespace editor::commands::emitters;

Vector<Ref<editor::commands::Command<node::IsometricMap>>> DragAndDropCommandEmitter::from_gui_input_to_command_impl([[maybe_unused]] Ref<InputEventMouse> p_event
) {// NOLINT(performance-unnecessary-value-param)
    Vector<Ref<editor::commands::Command<node::IsometricMap>>> commands;
    bool is_activated {initial_position != Vector3(-1, -1, -1)};

    int selected_tile_id {editor::IsometricEditorPlugin::get_instance()->get_selection_pane()->get_selected_positionable_id()};

    if (selected_tile_id == resource::PositionableSet::NONE_POSITIONABLE_ID) { return commands; }

    IsometricEditorPlugin* isometric_editor_plugin {IsometricEditorPlugin::get_instance()};
    node::IsometricMap* map {isometric_editor_plugin->get_selected_map()};

    const data::IsometricParameters* parameters {IsometricServer::get_instance()->space_get_configuration(map->get_space_RID())};

    EditorPlane& editor_plane = isometric_editor_plugin->get_editor_plane_for_map(map, EditorPlane::PlaneType::EDITOR_DRAWER);
    Vector3::Axis editor_plane_axis = editor_plane.get_axis();
    const Vector3& mouse_position {utils::from_screen_to_3D(
      *parameters,
      map->get_local_mouse_position(),
      editor_plane_axis,
      static_cast<float>(editor_plane.get_position())
    )};

    Vector3 positionable_size;
    if (auto* positionable {Object::cast_to<node::IsometricPositionable>(
          map->get_positionable_set()->get_positionable_scene_for_id(selected_tile_id)->instantiate()
        )}) {
        positionable_size = positionable->get_size();
        memdelete(positionable);
    } else {
        return commands;
    }

    if (Input::get_singleton()->is_mouse_button_pressed(MouseButton::LEFT)) {
        if (is_activated) {
            AABB real_aabb {_calculate_real_aabb(initial_position, mouse_position, positionable_size)};
            if (!map->is_aabb_in_map(real_aabb) || map->is_overlapping(real_aabb) || !isometric_editor_plugin->is_aabb_in_view_limiters(real_aabb)) {
                return commands;
            }

            limit_position = mouse_position;
        } else {
            AABB real_aabb {_calculate_real_aabb(mouse_position, mouse_position, positionable_size)};
            if (!map->is_aabb_in_map(real_aabb) || map->is_overlapping(real_aabb) || !isometric_editor_plugin->is_aabb_in_view_limiters(real_aabb)) {
                return commands;
            }

            initial_position = mouse_position;
            limit_position = mouse_position;
        }

        Vector<Vector3> all_positions {_calculate_positionables_positions(initial_position, limit_position, positionable_size)};

        int new_size = all_positions.size();

        if (preview_node->get_parent() == map) { map->remove_child(preview_node); }

        // Shrink, remove and delete excess nodes.
        _clear_current_preview_nodes(new_size);

        // Existing nodes are reused.
        for (int i = 0; i < current_preview_nodes.size(); ++i) {
            node::IsometricPositionable* preview_positionable {current_preview_nodes[i]};
            preview_positionable->set_modulate(Color(1, 1, 1, 0.5));
            preview_positionable->set_local_position_3d(all_positions[i]);
            preview_node->add_child(preview_positionable);
        }

        // New nodes are added if the size grew.
        for (int i = current_preview_nodes.size(); i < new_size; ++i) {
            node::IsometricPositionable* preview_positionable {Object::cast_to<node::IsometricPositionable>(
              map->get_positionable_set()->get_positionable_scene_for_id(selected_tile_id)->instantiate()
            )};
            current_preview_nodes.push_back(preview_positionable);
            preview_positionable->set_modulate(Color(1, 1, 1, 0.5));
            preview_positionable->set_local_position_3d(all_positions[i]);
            preview_node->add_child(preview_positionable);
        }

        map->add_child(preview_node);

        return commands;
    } else if (is_activated) {
        map->remove_child(preview_node);
        _clear_current_preview_nodes(0);

        Vector<Vector3> all_positions {_calculate_positionables_positions(initial_position, limit_position, positionable_size)};

        for (Vector3 position : all_positions) {
            Ref<editor::commands::AddPositionableCommand> add_command;
            add_command.instantiate();
            add_command->set_aabb({position, positionable_size});
            add_command->set_positionable_id(selected_tile_id);
            add_command->set_layer_id(IsometricEditorPlugin::get_instance()->get_selected_layer());
            commands.push_back(add_command);
        }

        initial_position = Vector3(-1, -1, -1);
        limit_position = Vector3(-1, -1, -1);
    }

    return commands;
}

void DragAndDropCommandEmitter::_clear_current_preview_nodes(int new_size) {
    if (IsometricEditorPlugin* isometric_editor_plugin = IsometricEditorPlugin::get_instance()) {
        node::IsometricMap* map {isometric_editor_plugin->get_selected_map()};

        // Only remove but not delete nodes in the new range
        for (int i = 0; i < MIN(new_size, current_preview_nodes.size()); ++i) {
            if (node::IsometricPositionable* current_preview_node = current_preview_nodes[i]) {
                preview_node->remove_child(current_preview_node);
            }
        }
        // Remove and delete excess nodes.
        for (int i = new_size; i < current_preview_nodes.size(); ++i) {
            if (node::IsometricPositionable* current_preview_node = current_preview_nodes[i]) {
                preview_node->remove_child(current_preview_node);
                memdelete(current_preview_node);
            }
        }
        if (new_size < current_preview_nodes.size()) { current_preview_nodes.resize(new_size); }
    }
}

AABB DragAndDropCommandEmitter::_calculate_real_aabb(const Vector3& initial_position, const Vector3& limit_position, const Vector3& positionable_size) {
    const Vector3& position_delta {limit_position - initial_position};

    float x_size {position_delta.x};
    float positionable_x_size {positionable_size.x};
    if (x_size < positionable_x_size) {
        x_size = positionable_x_size;
    } else {
        x_size = Math::floor(position_delta.x / positionable_x_size) * positionable_x_size + 1;
    }

    float y_size {position_delta.y};
    float positionable_y_size {positionable_size.y};
    if (y_size < positionable_y_size) {
        y_size = positionable_y_size;
    } else {
        y_size = Math::floor(position_delta.y / positionable_y_size) * positionable_y_size + 1;
    }

    float z_size {position_delta.z};
    float positionable_z_size {positionable_size.z};
    if (z_size < positionable_z_size) {
        z_size = positionable_z_size;
    } else {
        z_size = Math::floor(position_delta.z / positionable_z_size) * positionable_z_size + 1;
    }

    switch (
      IsometricEditorPlugin::get_instance()->get_editor_plane_for_map(
                                             IsometricEditorPlugin::get_instance()->get_selected_map(),
                                             EditorPlane::PlaneType::EDITOR_DRAWER
      ).get_axis()
    ) {
        case Vector3::AXIS_X:
            return {initial_position, {positionable_x_size, y_size, z_size}};
        case Vector3::AXIS_Y:
            return {initial_position, {x_size, positionable_y_size, z_size}};
        case Vector3::AXIS_Z:
            return {initial_position, {x_size, y_size, positionable_z_size}};
    }

    return {};
}

Vector<Vector3> DragAndDropCommandEmitter::_calculate_positionables_positions(
  const Vector3& initial_position,
  const Vector3& limit_position,
  const Vector3& positionable_size
) {
    AABB real_aabb {_calculate_real_aabb(initial_position, limit_position, positionable_size)};

    int elements_count_on_x {static_cast<int>(real_aabb.size.x / positionable_size.x)};
    int elements_count_on_y {static_cast<int>(real_aabb.size.y / positionable_size.y)};
    int elements_count_on_z {static_cast<int>(real_aabb.size.z / positionable_size.z)};

    Vector<Vector3> r_ret;

    for (int i = 0; i < elements_count_on_x; ++i) {
        for (int j = 0; j < elements_count_on_y; ++j) {
            for (int k = 0; k < elements_count_on_z; ++k) {
                r_ret.push_back(
                  initial_position + Vector3(1, 0, 0) * static_cast<float>(i) * positionable_size.x
                  + Vector3(0, 1, 0) * static_cast<float>(j) * positionable_size.y
                  + Vector3(0, 0, 1) * static_cast<float>(k) * positionable_size.z
                );
            }
        }
    }

    return r_ret;
}

DragAndDropCommandEmitter::DragAndDropCommandEmitter() :
  CommandEmitter(),
  current_preview_nodes(),
  initial_position(-1, -1, -1),
  limit_position(-1, -1, -1),
  preview_node(memnew(Node)) {}

DragAndDropCommandEmitter::~DragAndDropCommandEmitter() {
    _clear_current_preview_nodes(0);
    memdelete(preview_node);
    preview_node = nullptr;
}

#endif
