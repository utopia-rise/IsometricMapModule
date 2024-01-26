#ifdef TOOLS_ENABLED
#include "select_positionable_command.h"

#include "editor/isometric_editor_plugin.h"
#include "editor/positionable_selector_manager.h"

using namespace editor::commands;

void SelectPositionableCommand::redo() {
    if (should_deselect_first) {
        selected_cache = editor::PositionableSelectorManager::get_instance().get_selected_for_map(context_node);
        editor::PositionableSelectorManager::get_instance().deselect_all(context_node);
    }
    node::IsometricPositionable* positionable {context_node->get_positionable_at(position)};
    if (editor::PositionableSelectorManager::get_instance().is_position_selected_for_map(context_node, position)) {
        editor::PositionableSelectorManager::get_instance().deselect_positionable_at(context_node, position);
    } else {
        editor::PositionableSelectorManager::get_instance().select_positionable_at(context_node, positionable);
    }
}

void SelectPositionableCommand::undo() {
    if (editor::PositionableSelectorManager::get_instance().is_position_selected_for_map(context_node, position)) {
        editor::PositionableSelectorManager::get_instance().deselect_positionable_at(context_node, position);
    } else {
        if (node::IsometricPositionable * positionable {context_node->get_positionable_at(position)}) {
            editor::PositionableSelectorManager::get_instance().select_positionable_at(context_node, positionable);
        }
    }
    if (should_deselect_first) {
        editor::PositionableSelectorManager::get_instance().set_selected_for_map(context_node, selected_cache);
    }
}

void SelectPositionableCommand::set_position(const Vector3& p_position) {
    position = p_position;
}

void SelectPositionableCommand::set_should_deselect_first(bool p_should) {
    should_deselect_first = p_should;
}

SelectPositionableCommand::SelectPositionableCommand() : should_deselect_first(false) {}

void SelectPositionableCommand::_bind_methods_impl() {}

#endif