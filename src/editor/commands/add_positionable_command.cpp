#ifdef TOOLS_ENABLED

#include "add_positionable_command.h"

using namespace editor::commands;

void AddPositionableCommand::set_position(const Vector3& p_position) {
    position = p_position;
}

void AddPositionableCommand::set_positionable_id(int id) {
    positionable_id = id;
}

void AddPositionableCommand::set_map(node::IsometricMap* p_map) {
    map = p_map;
}

void AddPositionableCommand::redo_implementation() {
    map->add_positionable_if_nothing_present(position, positionable_id);
}

void AddPositionableCommand::undo_implementation() {
    map->remove_positionable(position);
}

void AddPositionableCommand::_bind_methods_impl() {

}

AddPositionableCommand::AddPositionableCommand() : Command<AddPositionableCommand>(),
        position(),
        positionable_id(resource::PositionableSet::NONE_VALUE),
        map(nullptr) {

}

#endif
