#ifdef TOOLS_ENABLED

#include "move_editor_view_limiter_command_emitter.h"
#include <modules/isometric_maps/src/editor/commands/composite_command.h>
#include <modules/isometric_maps/src/editor/commands/set_positionable_visibility_command.h>
#include <core/os/keyboard.h>
#include <core/os/input.h>
#include <modules/isometric_maps/src/editor/isometric_editor_plugin.h>
#include <modules/isometric_maps/src/editor/commands/move_editor_plane_command.h>

using namespace editor::commands::emitters;

MoveEditorViewLimiterCommandEmitter::MoveEditorViewLimiterCommandEmitter(UndoRedo* undo_redo)
        : CommandEmitter(undo_redo) {

}

Vector<Ref<editor::commands::Command>> MoveEditorViewLimiterCommandEmitter::from_gui_input_to_command_impl(
        Ref<InputEventMouseButton> p_event) {
    Vector<Ref<editor::commands::Command>> commands;

    if (!p_event->is_pressed() ||
        !(p_event->get_control() || p_event->get_command())) {
        return commands;
    }

    bool is_forward{false};
    switch (p_event->get_button_index()) {
        case ButtonList::BUTTON_WHEEL_UP:
            is_forward = true;
            break;
        case ButtonList::BUTTON_WHEEL_DOWN:
            break;
        default:
            return commands;
    }

    EditorPlane::PlaneType plane_type;
    int plane_max_position;
    if (Input::get_singleton()->is_key_pressed(KeyList::KEY_1)) {
        plane_type = p_event->get_shift() ?
                EditorPlane::PlaneType::X_MIN_VIEW_LIMITER :
                EditorPlane::PlaneType::X_MAX_VIEW_LIMITER ;
        plane_max_position = IsometricEditorPlugin::get_instance()->get_selected_map()->get_size().x;
    } else if (Input::get_singleton()->is_key_pressed(KeyList::KEY_2)) {
        plane_type = p_event->get_shift() ?
                     EditorPlane::PlaneType::Y_MIN_VIEW_LIMITER :
                     EditorPlane::PlaneType::Y_MAX_VIEW_LIMITER ;
        plane_max_position = IsometricEditorPlugin::get_instance()->get_selected_map()->get_size().y;
    } else if (Input::get_singleton()->is_key_pressed(KeyList::KEY_3)) {
        plane_type = p_event->get_shift() ?
                     EditorPlane::PlaneType::Z_MIN_VIEW_LIMITER :
                     EditorPlane::PlaneType::Z_MAX_VIEW_LIMITER ;
        plane_max_position = IsometricEditorPlugin::get_instance()->get_selected_map()->get_size().z;
    } else {
        return commands;
    }

    const EditorPlane plane{IsometricEditorPlugin::get_instance()
                                    ->get_editor_plane_for_selected_map(plane_type)};

    auto editor_plane_position{static_cast<real_t>(plane.get_position())};

    if ((editor_plane_position == 0 && !is_forward) || (editor_plane_position == plane_max_position && is_forward)) {
        return commands;
    }

    const EditorPlane& x_min_view_limiter{IsometricEditorPlugin::get_instance()->get_editor_plane_for_selected_map(EditorPlane::PlaneType::X_MIN_VIEW_LIMITER)};
    const EditorPlane& x_max_view_limiter{IsometricEditorPlugin::get_instance()->get_editor_plane_for_selected_map(EditorPlane::PlaneType::X_MAX_VIEW_LIMITER)};
    const EditorPlane& y_min_view_limiter{IsometricEditorPlugin::get_instance()->get_editor_plane_for_selected_map(EditorPlane::PlaneType::Y_MIN_VIEW_LIMITER)};
    const EditorPlane& y_max_view_limiter{IsometricEditorPlugin::get_instance()->get_editor_plane_for_selected_map(EditorPlane::PlaneType::Y_MAX_VIEW_LIMITER)};
    const EditorPlane& z_min_view_limiter{IsometricEditorPlugin::get_instance()->get_editor_plane_for_selected_map(EditorPlane::PlaneType::Z_MIN_VIEW_LIMITER)};
    const EditorPlane& z_max_view_limiter{IsometricEditorPlugin::get_instance()->get_editor_plane_for_selected_map(EditorPlane::PlaneType::Z_MAX_VIEW_LIMITER)};
    real_t range_to_change_visibility_position{is_forward ? editor_plane_position : editor_plane_position - 1};
    Ref<CompositeCommand> composite_command;
    composite_command.instance();
    switch (plane.get_axis()) {
        case Vector3::AXIS_X: {
            for (int y = y_min_view_limiter.get_position(); y < y_max_view_limiter.get_position(); ++y) {
                for (int z = z_min_view_limiter.get_position(); z < z_max_view_limiter.get_position(); ++z) {
                    Vector3 position{
                            range_to_change_visibility_position,
                            static_cast<real_t>(y),
                            static_cast<real_t>(z)
                    };

                    if (!IsometricEditorPlugin::get_instance()->get_selected_map()->get_positionable_at(position)) {
                        continue;
                    }

                    Ref<editor::commands::SetPositionableVisibilityCommand> visibility_command;
                    visibility_command.instance();
                    visibility_command->set_position(position);
                    composite_command->append_command(visibility_command);
                }
            }
        }
            break;
        case Vector3::AXIS_Y:
            for (int x = x_min_view_limiter.get_position(); x < x_max_view_limiter.get_position(); ++x) {
                for (int z = z_min_view_limiter.get_position(); z < z_max_view_limiter.get_position(); ++z) {
                    Vector3 position{
                            static_cast<real_t>(x),
                            range_to_change_visibility_position,
                            static_cast<real_t>(z)
                    };

                    if (!IsometricEditorPlugin::get_instance()->get_selected_map()->get_positionable_at(position)) {
                        continue;
                    }

                    Ref<editor::commands::SetPositionableVisibilityCommand> visibility_command;
                    visibility_command.instance();
                    visibility_command->set_position(position);
                    composite_command->append_command(visibility_command);
                }
            }
            break;
        case Vector3::AXIS_Z:
            for (int x = x_min_view_limiter.get_position(); x < x_max_view_limiter.get_position(); ++x) {
                for (int y = y_min_view_limiter.get_position(); y < y_max_view_limiter.get_position(); ++y) {
                    Vector3 position{
                            static_cast<real_t>(x),
                            static_cast<real_t>(y),
                            range_to_change_visibility_position
                    };

                    if (!IsometricEditorPlugin::get_instance()->get_selected_map()->get_positionable_at(position)) {
                        continue;
                    }

                    Ref<editor::commands::SetPositionableVisibilityCommand> visibility_command;
                    visibility_command.instance();
                    visibility_command->set_position(position);
                    composite_command->append_command(visibility_command);
                }
            }
            break;
    }

    commands.push_back(composite_command);

    Ref<editor::commands::MoveEditorPlaneCommand> move_command;
    move_command.instance();
    move_command->set_is_forward(is_forward);
    move_command->set_plane_type(plane_type);
    commands.push_back(move_command);

    return commands;
}

#endif