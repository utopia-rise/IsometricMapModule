#ifdef TOOLS_ENABLED

#include "add_positionable_command.h"

using namespace editor::commands;

void AddPositionableCommand::set_aabb(const AABB& p_aabb) {
    aabb = p_aabb;
}

void AddPositionableCommand::set_positionable_id(int id) {
    positionable_id = id;
}

void AddPositionableCommand::set_map(node::IsometricMap* p_map) {
    map = p_map;
}

void AddPositionableCommand::redo_implementation() {
    map->add_positionable_if_nothing_present(aabb, positionable_id);
}

void AddPositionableCommand::undo_implementation() {
    map->remove_positionable(aabb);
}

void AddPositionableCommand::_bind_methods_impl() {

}

AddPositionableCommand::AddPositionableCommand() : Command<AddPositionableCommand>(),
                                                   aabb(),
                                                   positionable_id(resource::PositionableSet::NONE_POSITIONABLE_ID),
                                                   map(nullptr) {

}

#endif