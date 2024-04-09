#ifndef ISOMETRIC_MAPS_REVERT_COMMAND_H
#define ISOMETRIC_MAPS_REVERT_COMMAND_H

#ifdef TOOLS_ENABLED

#include "command.h"

namespace editor {
    namespace commands {
        template<class TContextNode>
        class RevertCommand : public Command<TContextNode> {
        public:
            void redo() override;
            void undo() override;
            void set_context_node(TContextNode* p_context_node) override;

            void set_reverse_command(Ref<Command<TContextNode>> p_reverse_command);

            RevertCommand() = default;
            ~RevertCommand() override = default;

        private:
            Ref<Command<TContextNode>> reverse_command;
        };

        template<class TContextNode>
        void RevertCommand<TContextNode>::redo() {
            reverse_command->undo();
            Command<TContextNode>::redo();
        }

        template<class TContextNode>
        void RevertCommand<TContextNode>::undo() {
            reverse_command->redo();
            Command<TContextNode>::undo();
        }

        template<class TContextNode>
        void RevertCommand<TContextNode>::set_context_node(TContextNode* p_context_node) {
            Command<TContextNode>::set_context_node(p_context_node);
            reverse_command->set_context_node(p_context_node);
        }

        template<class TContextNode>
        void RevertCommand<TContextNode>::set_reverse_command(Ref<Command<TContextNode>> p_reverse_command) {
            reverse_command = p_reverse_command;
        }
    }// namespace commands
}// namespace editor

#endif
#endif// ISOMETRIC_MAPS_REVERT_COMMAND_H
