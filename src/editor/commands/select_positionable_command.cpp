#ifdef TOOLS_ENABLED

    #include "select_positionable_command.h"
    #include <modules/isometric_maps/src/editor/isometric_editor_plugin.h>
    #include <modules/isometric_maps/src/editor/positionable_selector_manager.h>

using namespace editor::commands;

void SelectPositionableCommand::redo() {
    node::IsometricMap* map {IsometricEditorPlugin::get_instance()->get_selected_map()};
    if (should_deselect_first) {
        selected_cache = editor::PositionableSelectorManager::get_instance().get_selected_for_map(map);
        editor::PositionableSelectorManager::get_instance().deselect_all(map);
    }
    node::IsometricPositionable* positionable {map->get_positionable_at(position)};
    if (editor::PositionableSelectorManager::get_instance().is_position_selected_for_map(map, position)) {
        editor::PositionableSelectorManager::get_instance().deselect_positionable_at(map, position);
    } else {
        editor::PositionableSelectorManager::get_instance().select_positionable_at(map, positionable);
    }
}

void SelectPositionableCommand::undo() {
    node::IsometricMap* map {IsometricEditorPlugin::get_instance()->get_selected_map()};
    if (editor::PositionableSelectorManager::get_instance().is_position_selected_for_map(map, position)) {
        editor::PositionableSelectorManager::get_instance().deselect_positionable_at(map, position);
    } else {
        if (node::IsometricPositionable * positionable {map->get_positionable_at(position)}) {
            editor::PositionableSelectorManager::get_instance().select_positionable_at(map, positionable);
        }
    }
    if (should_deselect_first) { editor::PositionableSelectorManager::get_instance().set_selected_for_map(map, selected_cache); }
}

void SelectPositionableCommand::set_position(const Vector3& p_position) {
    position = p_position;
}

void SelectPositionableCommand::set_should_deselect_first(bool p_should) {
    should_deselect_first = p_should;
}

SelectPositionableCommand::SelectPositionableCommand() : Command(), position(), should_deselect_first(false), selected_cache() {}

void SelectPositionableCommand::_bind_methods_impl() {}

#endif