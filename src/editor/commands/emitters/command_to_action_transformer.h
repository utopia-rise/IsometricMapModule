#ifndef ISOMETRIC_MAPS_COMMAND_TO_ACTION_TRANSFORMER_H
#define ISOMETRIC_MAPS_COMMAND_TO_ACTION_TRANSFORMER_H

#ifdef TOOLS_ENABLED

#include <core/templates/vector.h>
#include <core/object/ref_counted.h>
#include <editor/commands/command.h>
#include <scene/main/node.h>

namespace editor {
    namespace commands {
        namespace emitters {
            class CommandToActionTransformer {
            public:
                template<const char* action_title, UndoRedo::MergeMode merge_mode = UndoRedo::MERGE_DISABLE>
                void transform(const Vector<Ref<Command>> commands, Node* p_context);

                CommandToActionTransformer() = default;
                ~CommandToActionTransformer() = default;
            };

            template<const char* action_title, UndoRedo::MergeMode merge_mode>
            void CommandToActionTransformer::transform(const Vector<Ref<Command>> commands, Node* p_context) {
                bool has_valid_command {false};
                for (int i = 0; i < commands.size(); ++i) {
                    Ref<Command> command {commands[i]};
                    if (command.is_null()) { continue; }
                    if (!has_valid_command) {
                        EditorUndoRedoManager::get_singleton()->create_action(action_title, merge_mode, p_context);
                        has_valid_command = true;
                    }
                    command->append_to_undoredo();
                }

                if (has_valid_command) { EditorUndoRedoManager::get_singleton()->commit_action(); }
            }
        }
    }
}

#endif


#endif //ISOMETRIC_MAPS_COMMAND_TO_ACTION_TRANSFORMER_H
