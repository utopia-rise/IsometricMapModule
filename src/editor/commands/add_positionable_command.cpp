#ifdef TOOLS_ENABLED

#include "add_positionable_command.h"

using namespace editor::commands;

void AddPositionableCommand::redo_implementation() {
    map->add_positionable_if_nothing_present(position, positionable_id);
}

void AddPositionableCommand::undo_implementation() {
    map->remove_positionable(position);
}

void AddPositionableCommand::_bind_methods_impl() {

}

AddPositionableCommand::AddPositionableCommand(const Vector3& p_position, int p_positionable_id, node::IsometricMap* p_map)
        : Command<AddPositionableCommand>(),
          position(p_position),
          positionable_id(p_positionable_id),
          map(p_map) {

}

#endif
