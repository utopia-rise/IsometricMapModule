#ifdef TOOLS_ENABLED

#include <modules/isometric_maps/src/data/isometric_parameters.h>
#include <modules/isometric_maps/src/isometric_server.h>
#include <modules/isometric_maps/src/utils/isometric_maths.h>
#include <modules/isometric_maps/src/editor/isometric_editor_plugin.h>
#include <core/os/input.h>
#include "drag_and_drop_command_emitter.h"

using namespace editor::commands::emitters;

Vector<Ref<editor::commands::Command>>
DragAndDropCommandEmitter::from_gui_input_to_command_impl(Ref<InputEventMouse> p_event) { // NOLINT(performance-unnecessary-value-param)
    Vector<Ref<editor::commands::Command>> commands;
    bool is_activated{initial_position != Vector3(-1, -1, -1)};

    int selected_tile_id{
        editor::IsometricEditorPlugin::get_instance()->get_selection_pane()->get_selected_positionable_id()
    };

    if (selected_tile_id == resource::PositionableSet::NONE_POSITIONABLE_ID) {
        return commands;
    }

    IsometricEditorPlugin* isometric_editor_plugin{IsometricEditorPlugin::get_instance()};
    node::IsometricMap* map{isometric_editor_plugin->get_selected_map()};

    const data::IsometricParameters* parameters{
        IsometricServer::get_instance()->get_space_configuration(map->get_space_RID())
    };

    const Vector3& mouse_position{
        utils::from_screen_to_3D(
                *parameters,
                map->get_local_mouse_position(),
                static_cast<float>(isometric_editor_plugin->get_editor_plane_for_selected_map().get_position())
        )
    };

    Vector3 positionable_size;
    if (auto* positionable{
        Object::cast_to<node::IsometricPositionable>(
                map->get_positionable_set()->get_positionable_scene_for_id(selected_tile_id)->instance()
        )
    }
    ) {
        positionable_size = positionable->get_size();
        memdelete(positionable);
    } else {
        return commands;
    }

    if (Input::get_singleton()->is_mouse_button_pressed(BUTTON_LEFT)) {
        if (is_activated) {
            AABB real_aabb{_calculate_real_aabb(initial_position, mouse_position, positionable_size)};
            if (!map->is_aabb_in_map(real_aabb) || map->is_overlapping(real_aabb)) {
                return commands;
            }

            limit_position = mouse_position;
        } else {
            AABB real_aabb{_calculate_real_aabb(mouse_position, mouse_position, positionable_size)};
            if (!map->is_aabb_in_map(real_aabb) || map->is_overlapping(real_aabb)) {
                return commands;
            }

            initial_position = mouse_position;
            limit_position = mouse_position;
        }

        _clear_current_preview_nodes();

        Vector<Vector3> all_positions{_calculate_positionables_positions(initial_position, limit_position, positionable_size)};

        for (int i = 0; i < all_positions.size(); ++i) {
            node::IsometricPositionable* preview_positionable{
                Object::cast_to<node::IsometricPositionable>(
                        map->get_positionable_set()->get_positionable_scene_for_id(selected_tile_id)->instance()
                )
            };
            preview_positionable->set_modulate(Color(1, 1, 1, 0.5));
            map->add_child(preview_positionable);
            preview_positionable->set_local_position_3d(all_positions[i]);
            current_preview_nodes.push_back(preview_positionable);
        }

        return commands;
    } else if (is_activated) {
        _clear_current_preview_nodes();

        Vector<Vector3> all_positions{_calculate_positionables_positions(initial_position, limit_position, positionable_size)};

        for (int i = 0; i < all_positions.size(); ++i) {
            Ref<editor::commands::AddPositionableCommand> add_command;
            add_command.instance();
            add_command->set_aabb({all_positions[i], positionable_size});
            add_command->set_positionable_id(selected_tile_id);
            commands.push_back(add_command);
        }

        initial_position = Vector3(-1, -1, -1);
        limit_position = Vector3(-1, -1, -1);
    }

    return commands;
}

void DragAndDropCommandEmitter::_clear_current_preview_nodes() {
    if (IsometricEditorPlugin* isometric_editor_plugin{IsometricEditorPlugin::get_instance()}) {
        node::IsometricMap* map{isometric_editor_plugin->get_selected_map()};
        for (int i = 0; i < current_preview_nodes.size(); ++i) {
            if (node::IsometricPositionable* current_preview_node{current_preview_nodes[i]}) {
                map->remove_child(current_preview_node);
                memdelete(current_preview_node);
            }
        }
        current_preview_nodes.resize(0);
    }
}

AABB DragAndDropCommandEmitter::_calculate_real_aabb(const Vector3& initial_position, const Vector3& limit_position,
                                                     const Vector3& positionable_size) {
    const Vector3& position_delta{limit_position - initial_position};

    float x_size{position_delta.x};
    float positionable_x_size{positionable_size.x};
    if (x_size < positionable_x_size) {
        x_size = positionable_x_size;
    } else {
        x_size = Math::floor(position_delta.x / positionable_x_size) * positionable_x_size + 1;
    }

    float  y_size{position_delta.y};
    float positionable_y_size{positionable_size.y};
    if (y_size < positionable_y_size) {
        y_size = positionable_y_size;
    } else {
        y_size = Math::floor(position_delta.y / positionable_y_size) * positionable_y_size + 1;
    }

    return {initial_position, {x_size, y_size, positionable_size.z}};
}

Vector<Vector3> DragAndDropCommandEmitter::_calculate_positionables_positions(const Vector3& initial_position,
                                                                              const Vector3& limit_position,
                                                                              const Vector3& positionable_size) {
    AABB real_aabb{_calculate_real_aabb(initial_position, limit_position, positionable_size)};

    int elements_count_on_x{static_cast<int>(real_aabb.size.x / positionable_size.x)};
    int elements_count_on_y{static_cast<int>(real_aabb.size.y / positionable_size.y)};

    Vector<Vector3> r_ret;

    for (int i = 0; i < elements_count_on_x; ++i) {
        for (int j = 0; j < elements_count_on_y; ++j) {
            r_ret.push_back(
                    initial_position +
                    Vector3(1, 0, 0) * static_cast<float>(i) * positionable_size.x +
                    Vector3(0, 1, 0) * static_cast<float>(j) * positionable_size.y
            );
        }
    }

    return r_ret;
}

DragAndDropCommandEmitter::DragAndDropCommandEmitter(UndoRedo* undo_redo) : CommandEmitter(undo_redo),
                                                                            current_preview_nodes(),
                                                                            initial_position(-1, -1, -1),
                                                                            limit_position(-1, -1, -1) {

}

DragAndDropCommandEmitter::~DragAndDropCommandEmitter() {
    _clear_current_preview_nodes();
}

#endif
