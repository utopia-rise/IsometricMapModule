#ifndef ISOMETRIC_MAPS_COMPOSITE_COMMAND_H
#define ISOMETRIC_MAPS_COMPOSITE_COMMAND_H

#ifdef TOOLS_ENABLED

#include "command.h"

namespace editor {
    namespace commands {
        template<class TContextNode>
        class CompositeCommand : public Command<TContextNode> {
        public:
            void redo() override;
            void undo() override;
            void set_context_node(TContextNode* p_context_node) override;

            void append_command(Ref<Command<TContextNode>> command);

            CompositeCommand() = default;
            ~CompositeCommand() override = default;

        private:
            Vector<Ref<Command<TContextNode>>> commands;
        };

        template<class TContextNode>
        void editor::commands::CompositeCommand<TContextNode>::redo() {
            for (int i = 0; i < commands.size(); ++i) {
                commands.get(i)->redo();
            }
            Command<TContextNode>::redo();
        }

        template<class TContextNode>
        void CompositeCommand<TContextNode>::undo() {
            for (int i = commands.size() - 1; i >= 0; --i) {
                commands.get(i)->undo();
            }
            Command<TContextNode>::undo();
        }

        template<class TContextNode>
        void CompositeCommand<TContextNode>::set_context_node(TContextNode* p_context_node) {
            Command<TContextNode>::set_context_node(p_context_node);
            for (const Ref<Command<TContextNode>>& command : commands) {
                command->set_context_node(p_context_node);
            }
        }

        template<class TContextNode>
        void CompositeCommand<TContextNode>::append_command(Ref<Command<TContextNode>> command) {// NOLINT(performance-unnecessary-value-param)
            commands.push_back(command);
        }
    }// namespace commands
}// namespace editor

#endif
#endif// ISOMETRIC_MAPS_COMPOSITE_COMMAND_H
