#ifdef TOOLS_ENABLED

#include "revert_command.h"

using namespace editor::commands;

void RevertCommand::redo() {
    reverse_command->undo();
}

void RevertCommand::undo() {
    reverse_command->redo();
}

void RevertCommand::set_reverse_command(Ref<Command> p_reverse_command) {
    reverse_command = p_reverse_command;
}

#endif
