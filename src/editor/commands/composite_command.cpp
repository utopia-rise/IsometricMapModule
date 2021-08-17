#ifdef TOOLS_ENABLED

#include "composite_command.h"

using namespace editor::commands;

void editor::commands::CompositeCommand::redo() {
    for (int i = 0; i < commands.size(); ++i) {
        commands.get(i)->redo();
    }
}

void CompositeCommand::undo() {
    for (int i = commands.size() - 1; i >= 0; --i) {
        commands.get(i)->undo();
    }
}

void CompositeCommand::append_command(Ref<Command> command) { // NOLINT(performance-unnecessary-value-param)
    commands.push_back(command);
}

#endif
