#ifndef ISOMETRIC_MAPS_REVERT_COMMAND_H
#define ISOMETRIC_MAPS_REVERT_COMMAND_H

#ifdef TOOLS_ENABLED

    #include "command.h"

namespace editor {
    namespace commands {
        class RevertCommand : public Command {
        public:
            void redo() override;
            void undo() override;

            void set_reverse_command(Ref<Command> p_reverse_command);

            RevertCommand() = default;
            ~RevertCommand() override = default;

        private:
            Ref<Command> reverse_command;
        };
    }// namespace commands
}// namespace editor

#endif
#endif// ISOMETRIC_MAPS_REVERT_COMMAND_H
