#ifdef TOOLS_ENABLED
#include "rotate_editor_plane_command_emitter.h"

#include "editor/commands/rotate_editor_plane_command.h"
#include "editor/isometric_editor_plugin.h"

using namespace editor::commands::emitters;

Vector<Ref<editor::commands::Command<node::IsometricMap>>> RotateEditorPlaneCommandEmitter::from_gui_input_to_command_impl(Ref<InputEventKey> p_event) {
    Vector<Ref<Command<node::IsometricMap>>> commands;

    if (!p_event->is_pressed()) { return commands; }

    EditorPlane& editor_plane {
      IsometricEditorPlugin::get_instance()->get_editor_plane_for_map(
        IsometricEditorPlugin::get_instance()->get_selected_map(),
        EditorPlane::PlaneType::EDITOR_DRAWER
      )
    };
    Vector3::Axis current_axis {editor_plane.get_axis()};

    Ref<editor::commands::RotateEditorPlaneCommand> rotate_command;
    int new_axis;
    switch (p_event->get_keycode()) {
        case Key::LEFT:
            new_axis = current_axis - 1;
            break;
        case Key::RIGHT:
            new_axis = current_axis + 1;
            break;
        default:
            return commands;
    }

    const int axis_count = 3;
    new_axis = Math::posmod(new_axis, axis_count);

    rotate_command.instantiate();
    rotate_command->set_former_axis(current_axis);
    rotate_command->set_former_position(editor_plane.get_position());
    rotate_command->set_new_axis(static_cast<Vector3::Axis>(new_axis));
    commands.push_back(rotate_command);

    return commands;
}

#endif