#ifdef TOOLS_ENABLED

#include "add_positionable_command.h"

using namespace editor::commands;

void AddPositionableCommand::redo_implementation() {
    //TODO
}

void AddPositionableCommand::undo_implementation() {
    //TODO
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
