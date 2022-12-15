#ifdef TOOLS_ENABLED
    #include "add_positionable_command.h"

    #include "editor/isometric_editor_plugin.h"

using namespace editor::commands;

void AddPositionableCommand::redo() {
    IsometricEditorPlugin::get_instance()->get_selected_map()->add_positionable_if_nothing_present(aabb, positionable_id);
}

void AddPositionableCommand::undo() {
    IsometricEditorPlugin::get_instance()->get_selected_map()->remove_positionable(aabb);
}

void AddPositionableCommand::set_aabb(const AABB& p_aabb) {
    aabb = p_aabb;
}

void AddPositionableCommand::set_positionable_id(int id) {
    positionable_id = id;
}

void AddPositionableCommand::_bind_methods_impl() {}

AddPositionableCommand::AddPositionableCommand() :
  Command(),
  aabb(),
  positionable_id(resource::PositionableSet::NONE_POSITIONABLE_ID) {}

#endif
