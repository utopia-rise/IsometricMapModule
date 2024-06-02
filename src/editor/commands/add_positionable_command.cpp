#ifdef TOOLS_ENABLED
#include "add_positionable_command.h"

#include "editor/isometric_editor_plugin.h"

using namespace editor::commands;

void AddPositionableCommand::redo() {
    if (is_overlapping) {
        context_node->add_positionable(aabb.position, positionable_id, layer_id);
    } else {
        context_node->add_positionable_if_nothing_present(aabb, positionable_id, layer_id);
    }
    Command<node::IsometricMap>::redo();
}

void AddPositionableCommand::undo() {
    is_overlapping = context_node->is_overlapping(aabb);
    context_node->remove_positionable(aabb);
    Command<node::IsometricMap>::undo();
}

void AddPositionableCommand::set_aabb(const AABB& p_aabb) {
    aabb = p_aabb;
}

void AddPositionableCommand::set_positionable_id(int id) {
    positionable_id = id;
}

void AddPositionableCommand::set_layer_id(uint32_t p_layer_id) {
    layer_id = p_layer_id;
}

AddPositionableCommand::AddPositionableCommand() :
  positionable_id(resource::PositionableSet::NONE_POSITIONABLE_ID),
  layer_id(node::IsometricMap::DEFAULT_LAYER_ID),
  is_overlapping(false) {}

#endif
