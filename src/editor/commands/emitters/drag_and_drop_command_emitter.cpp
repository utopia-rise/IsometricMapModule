#ifdef TOOLS_ENABLED
#include "drag_and_drop_command_emitter.h"

#include "data/isometric_parameters.h"
#include "editor/isometric_editor_plugin.h"
#include "isometric_server.h"
#include "utils/isometric_maths.h"

using namespace editor::commands::emitters;

static constexpr const uint32_t CHUNK_SIDE_SIZE {10};

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
        Vector3i former_limit_position;

        if (is_activated) {
            AABB real_aabb {_calculate_real_aabb(initial_position, mouse_position, positionable_size)};
            if (!map->is_aabb_in_map(real_aabb) || map->is_overlapping(real_aabb) || !isometric_editor_plugin->is_aabb_in_view_limiters(real_aabb)) {
                return commands;
            }

            former_limit_position = limit_position;
            limit_position = mouse_position;
        } else {
            AABB real_aabb {_calculate_real_aabb(mouse_position, mouse_position, positionable_size)};
            if (!map->is_aabb_in_map(real_aabb) || map->is_overlapping(real_aabb) || !isometric_editor_plugin->is_aabb_in_view_limiters(real_aabb)) {
                return commands;
            }

            former_limit_position = mouse_position;
            initial_position = mouse_position;
            limit_position = mouse_position;
        }
        
        Vector<Chunk*> chunks {
          _get_chunks_for_position_interval(
            initial_position,
            {
              MAX(limit_position.x, former_limit_position.x),
              MAX(limit_position.y, former_limit_position.y),
              MAX(limit_position.z, former_limit_position.z)
            }
          )
        };

        Vector3i begin_check_position {
          MIN(former_limit_position.x, static_cast<int>(limit_position.x)),
          MIN(former_limit_position.y, static_cast<int>(limit_position.y)),
          MIN(former_limit_position.z, static_cast<int>(limit_position.z))
        };

        Vector3i end_check_position {
          MAX(former_limit_position.x, static_cast<int>(limit_position.x)),
          MAX(former_limit_position.y, static_cast<int>(limit_position.y)),
          MAX(former_limit_position.z, static_cast<int>(limit_position.z))
        };

        for (Chunk* chunk : chunks) {
            if (chunk->last_position.x < begin_check_position.x &&
                chunk->last_position.y < begin_check_position.y &&
                chunk->last_position.z < begin_check_position.z) {
                continue;
            }

            if (chunk->begin_position.x > end_check_position.x &&
                chunk->begin_position.y > end_check_position.y &&
                chunk->begin_position.z > end_check_position.z) {
                continue;
            }

            Vector3 chunk_initial_position {
              MAX(chunk->begin_position.x, initial_position.x),
              MAX(chunk->begin_position.y, initial_position.y),
              MAX(chunk->begin_position.z, initial_position.z)
            };
            Vector3 chunk_limit_position {
              MIN(chunk->last_position.x, limit_position.x),
              MIN(chunk->last_position.y, limit_position.y),
              MIN(chunk->last_position.z, limit_position.z)
            };

            Vector<Vector3> all_positions;

            if (chunk_initial_position.x <= chunk_limit_position.x &&
                chunk_initial_position.y <= chunk_limit_position.y &&
                chunk_initial_position.z <= chunk_limit_position.z) {
                all_positions = _calculate_positionables_positions(chunk_initial_position, chunk_limit_position, positionable_size);
            }

            int new_size = all_positions.size();

            Node* preview_node {chunk->preview_node};
            if (preview_node->get_parent() == map) { map->remove_child(preview_node); }

            // Shrink, remove and delete excess nodes.
            _clear_current_preview_nodes(chunk, new_size);

            Vector<node::IsometricPositionable*>& current_preview_nodes {chunk->current_preview_nodes};

            // Existing nodes are reused.
            for (int i = 0; i < current_preview_nodes.size(); ++i) {
                node::IsometricPositionable* preview_positionable {current_preview_nodes[i]};
                preview_positionable->set_modulate(Color(1, 1, 1, 0.5));
                preview_positionable->set_local_position_3d(all_positions[i]);
                preview_node->add_child(preview_positionable);
            }

            // New nodes are added if the size grew.
            for (int i = current_preview_nodes.size(); i < new_size; ++i) {
                node::IsometricPositionable* preview_positionable {
                  Object::cast_to<node::IsometricPositionable>(
                    map->get_positionable_set()->get_positionable_scene_for_id(selected_tile_id)->instantiate()
                  )
                };
                current_preview_nodes.push_back(preview_positionable);
                preview_positionable->set_modulate(Color(1, 1, 1, 0.5));
                preview_positionable->set_local_position_3d(all_positions[i]);
                preview_node->add_child(preview_positionable);
            }

            map->add_child(preview_node);
        }

        return commands;
    } else if (is_activated) {
        for (const KeyValue<Vector2i, Chunk*>& item : preview_chunks) {
            Chunk* chunk {item.value};

            Node* preview_node {chunk->preview_node};
            if (preview_node->get_parent() == map) { map->remove_child(preview_node); }
            _clear_current_preview_nodes(chunk, 0);

            Vector3 chunk_initial_position {
              MAX(chunk->begin_position.x, initial_position.x),
              MAX(chunk->begin_position.y, initial_position.y),
              MAX(chunk->begin_position.z, initial_position.z)
            };
            Vector3 chunk_limit_position {
              MIN(chunk->last_position.x, limit_position.x),
              MIN(chunk->last_position.y, limit_position.y),
              MIN(chunk->last_position.z, limit_position.z)
            };

            if (chunk_initial_position.x > chunk_limit_position.x ||
                chunk_initial_position.y > chunk_limit_position.y ||
                chunk_initial_position.z > chunk_limit_position.z) {
                continue;
            }

            Vector<Vector3> all_positions {_calculate_positionables_positions(chunk_initial_position, chunk_limit_position, positionable_size)};

            for (Vector3 position : all_positions) {
                Ref<editor::commands::AddPositionableCommand> add_command;
                add_command.instantiate();
                add_command->set_aabb({position, positionable_size});
                add_command->set_positionable_id(selected_tile_id);
                add_command->set_layer_id(IsometricEditorPlugin::get_instance()->get_selected_layer());
                commands.push_back(add_command);
            }
        }

        for (KeyValue<Vector2i, Chunk*>& item : preview_chunks) {
            Chunk* chunk {item.value};
            memdelete(chunk->preview_node);
            delete chunk;
        }

        preview_chunks.clear();

        initial_position = Vector3(-1, -1, -1);
        limit_position = Vector3(-1, -1, -1);
    }

    return commands;
}

void DragAndDropCommandEmitter::_clear_current_preview_nodes(Chunk* p_chunk, int new_size) {
    Vector<node::IsometricPositionable*>& current_preview_nodes {p_chunk->current_preview_nodes};
    Node* preview_node {p_chunk->preview_node};
    
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

Vector<DragAndDropCommandEmitter::Chunk*> DragAndDropCommandEmitter::_get_chunks_for_position_interval(const Vector3& p_initial_position, const Vector3& p_target_position) {
    Vector<DragAndDropCommandEmitter::Chunk*> result;

    Vector2i initial_chunk_position {_get_chunk_position(p_initial_position)};
    Vector2i target_chunk_position {_get_chunk_position(p_target_position)};

    Vector2i difference {target_chunk_position - initial_chunk_position};
    Vector2i difference_sign {difference.sign()};

    Vector2i current_position {initial_chunk_position};

    bool is_only_one_on_x {current_position.x == target_chunk_position.x};
    bool is_only_one_on_y {current_position.y == target_chunk_position.y};

    while (current_position.x != target_chunk_position.x + difference_sign.x || is_only_one_on_x) {
        current_position.y = initial_chunk_position.y;
        while (current_position.y != target_chunk_position.y + difference_sign.y || is_only_one_on_y) {
            result.append(_get_or_create_chunks_for_position(current_position));

            if (is_only_one_on_y) {
                break;
            }

            current_position += Vector2i(0, difference_sign.y);
        }

        if (is_only_one_on_x) {
            break;
        }
        
        current_position += Vector2i(difference_sign.x, 0);
    }

    return result;
}

Vector2i DragAndDropCommandEmitter::_get_chunk_position(const Vector3& p_position) {
    Vector2i chunk_position;

    switch (
      IsometricEditorPlugin::get_instance()->get_editor_plane_for_map(
                                             IsometricEditorPlugin::get_instance()->get_selected_map(),
                                             EditorPlane::PlaneType::EDITOR_DRAWER
      ).get_axis()
    ) {
        case Vector3::AXIS_X:
            chunk_position = (Vector2(p_position.y, p_position.z) / CHUNK_SIDE_SIZE).floor();
            break;
        case Vector3::AXIS_Y:
            chunk_position = (Vector2(p_position.x, p_position.z) / CHUNK_SIDE_SIZE).floor();
            break;
        case Vector3::AXIS_Z:
            chunk_position = (Vector2(p_position.x, p_position.y) / CHUNK_SIDE_SIZE).floor();
            break;
    }

    return chunk_position;
}

DragAndDropCommandEmitter::Chunk* DragAndDropCommandEmitter::_get_or_create_chunks_for_position(const Vector2i& p_position) {
    if (DragAndDropCommandEmitter::Chunk** chunk {preview_chunks.getptr(p_position)}) {
        return *chunk;
    }
    
    DragAndDropCommandEmitter::Chunk* chunk {new Chunk()};
    chunk->current_preview_nodes = Vector<node::IsometricPositionable*>();
    chunk->preview_node = memnew(Node);

    EditorPlane& editor_drawer_plane = IsometricEditorPlugin::get_instance()->get_editor_plane_for_map(
      IsometricEditorPlugin::get_instance()->get_selected_map(),
      EditorPlane::PlaneType::EDITOR_DRAWER
    );
    switch (editor_drawer_plane.get_axis()
    ) {
        case Vector3::AXIS_X:
            chunk->begin_position = Vector3i(
              editor_drawer_plane.get_position(),
              static_cast<int>(p_position.x * CHUNK_SIDE_SIZE),
              static_cast<int>(p_position.y * CHUNK_SIDE_SIZE)
            );
            chunk->last_position = chunk->begin_position + Vector3i(0, CHUNK_SIDE_SIZE - 1, CHUNK_SIDE_SIZE - 1);
            break;
        case Vector3::AXIS_Y:
            chunk->begin_position = Vector3i(
              static_cast<int>(p_position.x * CHUNK_SIDE_SIZE),
              editor_drawer_plane.get_position(),
              static_cast<int>(p_position.y * CHUNK_SIDE_SIZE)
            );
            chunk->last_position = chunk->begin_position + Vector3i(CHUNK_SIDE_SIZE - 1, 0, CHUNK_SIDE_SIZE - 1);
            break;
        case Vector3::AXIS_Z:
            chunk->begin_position = Vector3i(
              static_cast<int>(p_position.x * CHUNK_SIDE_SIZE),
              static_cast<int>(p_position.y * CHUNK_SIDE_SIZE),
              editor_drawer_plane.get_position()
            );
            chunk->last_position = chunk->begin_position + Vector3i(CHUNK_SIDE_SIZE - 1, CHUNK_SIDE_SIZE - 1, 0);
            break;
    }

    return preview_chunks.insert(p_position, chunk)->value;
}

DragAndDropCommandEmitter::DragAndDropCommandEmitter() :
  CommandEmitter(),
  initial_position(-1, -1, -1),
  limit_position(-1, -1, -1) {}

DragAndDropCommandEmitter::~DragAndDropCommandEmitter() {
    for (KeyValue<Vector2i, Chunk*>& item : preview_chunks) {
        Chunk* chunk {item.value};
        _clear_current_preview_nodes(chunk, 0);
        memdelete(chunk->preview_node);
        delete chunk;
    }
    preview_chunks.clear();
}

#endif
