#ifndef ISOMETRIC_MAPS_COMPOSITE_COMMAND_H
#define ISOMETRIC_MAPS_COMPOSITE_COMMAND_H

#ifdef TOOLS_ENABLED

#include "command.h"

namespace editor {
    namespace commands {
        class CompositeCommand : public Command {
        public:
            void redo() override;
            void undo() override;

            void append_command(Ref<Command> command);

            CompositeCommand() = default;
            ~CompositeCommand() override = default;

        private:
            Vector<Ref<Command>> commands;
        };
    }// namespace commands
}// namespace editor

#endif
#endif// ISOMETRIC_MAPS_COMPOSITE_COMMAND_H
