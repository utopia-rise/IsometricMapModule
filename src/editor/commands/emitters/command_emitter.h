#ifndef ISOMETRIC_MAPS_COMMAND_EMITTER_H
#define ISOMETRIC_MAPS_COMMAND_EMITTER_H

#ifdef TOOLS_ENABLED
#include "../command.h"
#include "core/input/input_event.h"
#include "editor/editor_undo_redo_manager.h"
#include "command_to_action_transformer.h"
#include <scene/main/node.h>

namespace editor {
    namespace commands {
        namespace emitters {

            template<class Derived, class Evt, const char* action_title, UndoRedo::MergeMode merge_mode = UndoRedo::MERGE_DISABLE>
            class CommandEmitter {
            public:
                void on_gui_input(const Ref<InputEvent>& p_event, Node* p_context = nullptr);

                CommandEmitter() = default;
                ~CommandEmitter() = default;

            private:
                Vector<Ref<Command>> from_gui_input_to_command(Ref<Evt> p_event);
            };

            template<class Derived, class Evt, const char* action_title, UndoRedo::MergeMode merge_mode>
            void CommandEmitter<Derived, Evt, action_title, merge_mode>::on_gui_input(const Ref<InputEvent>& p_event, Node* p_context) {
                Ref<Evt> event {p_event};

                if (event.is_null()) {
                    return;
                }

                CommandToActionTransformer action_transformer;
                action_transformer.transform<action_title, merge_mode>(
                        from_gui_input_to_command(event),
                        p_context
                );
            }

            template<class Derived, class Evt, const char* action_title, UndoRedo::MergeMode merge_mode>
            Vector<Ref<Command>> CommandEmitter<Derived, Evt, action_title, merge_mode>::from_gui_input_to_command(Ref<Evt> p_event) {
                return reinterpret_cast<Derived*>(this)->from_gui_input_to_command_impl(p_event);
            }
        }// namespace emitters
    }// namespace commands
}// namespace editor

#endif
#endif// ISOMETRIC_MAPS_COMMAND_EMITTER_H
