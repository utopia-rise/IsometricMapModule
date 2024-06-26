#ifdef TOOLS_ENABLED
#include "set_positionable_visibility_command.h"

#include "editor/isometric_editor_plugin.h"

using namespace editor::commands;

void SetPositionableVisibilityCommand::redo() {
    if (node::IsometricPositionable * positionable {context_node->get_positionable_at(position)}) {
        positionable->set_visible(!positionable->is_visible());
    }
    Command<node::IsometricMap>::redo();
}

void SetPositionableVisibilityCommand::undo() {
    redo();
}

void SetPositionableVisibilityCommand::set_position(const Vector3& p_position) {
    position = p_position;
}

SetPositionableVisibilityCommand::SetPositionableVisibilityCommand() {}

#endif