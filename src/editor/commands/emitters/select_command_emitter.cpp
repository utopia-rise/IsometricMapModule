#ifdef TOOLS_ENABLED

#include <modules/isometric_maps/src/data/isometric_parameters.h>
#include <modules/isometric_maps/src/isometric_server.h>
#include <modules/isometric_maps/src/utils/isometric_maths.h>
#include <modules/isometric_maps/src/editor/isometric_editor_plugin.h>
#include "select_command_emitter.h"

using namespace editor::commands::emitters;

Vector<Ref<editor::commands::Command>>
SelectCommandEmitter::from_gui_input_to_command_impl(Ref<InputEventMouse> p_event) { // NOLINT(performance-unnecessary-value-param)
    Vector<Ref<Command>> commands;

    if (!p_event->is_pressed()) {
        return commands;
    }

    IsometricEditorPlugin* isometric_editor_plugin{IsometricEditorPlugin::get_instance()};
    node::IsometricMap* map{isometric_editor_plugin->get_selected_map()};

    const data::IsometricParameters* parameters{
        IsometricServer::get_instance()->get_space_configuration(map->get_space_RID())
    };

    EditorPlane& editor_plane = isometric_editor_plugin->get_editor_plane_for_selected_map(EditorPlane::PlaneType::EDITOR_DRAWER);
    const Vector3& position{
        utils::from_screen_to_3D(
                *parameters,
                map->get_local_mouse_position(),
                editor_plane.get_axis(),
                static_cast<float>(editor_plane.get_position())
        )
    };

    if (!map->get_positionable_at(position)) {
        return commands;
    }

    Ref<editor::commands::SelectPositionableCommand> command;
    command.instance();
    command->set_position(position);
    command->set_should_deselect_first(!p_event->get_control() && !p_event->get_command());

    commands.push_back(command);

    return commands;
}

SelectCommandEmitter::SelectCommandEmitter(UndoRedo* undo_redo) :
        CommandEmitter<SelectCommandEmitter, InputEventMouse>(undo_redo) {

}

#endif