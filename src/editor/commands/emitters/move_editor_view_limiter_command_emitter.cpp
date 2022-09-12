#ifdef TOOLS_ENABLED

#include "move_editor_view_limiter_command_emitter.h"
#include "modules/isometric_maps/src/logging.h"
#include "core/os/keyboard.h"
#include "core/os/input.h"
#include "modules/isometric_maps/src/editor/isometric_editor_plugin.h"
#include "modules/isometric_maps/src/editor/commands/move_editor_plane_command.h"

using namespace editor::commands::emitters;

MoveEditorViewLimiterCommandEmitter::MoveEditorViewLimiterCommandEmitter(UndoRedo* undo_redo,
                                                                         EditorPlane::PlaneType p_plane_type)
        : CommandEmitter(undo_redo), plane_type(p_plane_type), is_min(false) {
    switch (plane_type) {
        case EditorPlane::X_MIN_VIEW_LIMITER:
            input_key = KeyList::KEY_1;
            is_min = true;
            get_axis_size_method = [] {
                return IsometricEditorPlugin::get_instance()->get_selected_map()->get_size().x;
            };
            break;
        case EditorPlane::X_MAX_VIEW_LIMITER:
            input_key = KeyList::KEY_1;
            get_axis_size_method = [] {
                return IsometricEditorPlugin::get_instance()->get_selected_map()->get_size().x;
            };
            break;
        case EditorPlane::Y_MIN_VIEW_LIMITER:
            input_key = KeyList::KEY_2;
            is_min = true;
            get_axis_size_method = [] {
                return IsometricEditorPlugin::get_instance()->get_selected_map()->get_size().y;
            };
            break;
        case EditorPlane::Y_MAX_VIEW_LIMITER:
            input_key = KeyList::KEY_2;
            get_axis_size_method = [] {
                return IsometricEditorPlugin::get_instance()->get_selected_map()->get_size().y;
            };
            break;
        case EditorPlane::Z_MIN_VIEW_LIMITER:
            input_key = KeyList::KEY_3;
            is_min = true;
            get_axis_size_method = [] {
                return IsometricEditorPlugin::get_instance()->get_selected_map()->get_size().z;
            };
            break;
        case EditorPlane::Z_MAX_VIEW_LIMITER:
            input_key = KeyList::KEY_3;
            get_axis_size_method = [] {
                return IsometricEditorPlugin::get_instance()->get_selected_map()->get_size().z;
            };
            break;
        default:
            MAPS_CRASH_NOW_MSG(vformat("Cannot assign plane_type %s in a MoveEditorViewLimiterCommandEmitter", plane_type));
    }
}

Vector<Ref<editor::commands::Command>> MoveEditorViewLimiterCommandEmitter::from_gui_input_to_command_impl(
        Ref<InputEventMouseButton> p_event) {
    Vector<Ref<editor::commands::Command>> commands;
    if (!p_event->is_pressed() ||
        !(p_event->get_control() || p_event->get_command()) ||
        (is_min && !p_event->get_shift()) || (!is_min && p_event->get_shift()) ||
        !Input::get_singleton()->is_key_pressed(input_key)) {
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

    real_t editor_plane_position{
        static_cast<real_t>(
                IsometricEditorPlugin::get_instance()
                    ->get_editor_plane_for_selected_map(plane_type)
                    .get_position()
        )
    };

    if ((editor_plane_position == 0 && !is_forward) || (editor_plane_position == get_axis_size_method() && is_forward)) {
        return commands;
    }

    Ref<editor::commands::MoveEditorPlaneCommand> move_command;
    move_command.instance();
    move_command->set_is_forward(is_forward);
    move_command->set_plane_type(plane_type);
    commands.push_back(move_command);

    return commands;
}

#endif