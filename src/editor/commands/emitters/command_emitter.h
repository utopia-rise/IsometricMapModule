#ifdef TOOLS_ENABLED

#ifndef ISOMETRIC_MAPS_COMMANDEMITTER_H
#define ISOMETRIC_MAPS_COMMANDEMITTER_H

#include <core/undo_redo.h>
#include <core/os/input_event.h>
#include "painting_command_emitter.h"


namespace editor {
    namespace commands {
        namespace emitters {

            template<class Derived, class Command, class Evt>
            class CommandEmitter {
            public:
                void _on_gui_input(const Ref<InputEvent>& p_event);

                CommandEmitter() = delete;
                explicit CommandEmitter(UndoRedo* p_undo_redo);
                ~CommandEmitter() = default;

            private:
                UndoRedo* undo_redo;

                Vector<Ref<Command>> from_gui_input_to_command(Ref<Evt> p_event);
            };

            template<class Derived, class Command, class Evt>
            void CommandEmitter<Derived, Command, Evt>::_on_gui_input(const Ref<InputEvent>& p_event) {
                Ref<Evt> event{p_event};
                if (!event.is_null()) {
                    Vector<Ref<Command>> commands{from_gui_input_to_command(event)};

                    bool has_valid_command{false};
                    for (int i = 0; i < commands.size(); ++i) {
                        Ref<Command> command{commands[i]};
                        if (!command.is_valid()) continue;
                        if (!has_valid_command) {
                            undo_redo->create_action();
                        }
                        has_valid_command = true;
                        command->append_to_undoredo(undo_redo);
                    }

                    if (has_valid_command) {
                        undo_redo->commit_action();
                    }
                }
            }

            template<class Derived, class Command, class Evt>
            Vector<Ref<Command>> CommandEmitter<Derived, Command, Evt>::from_gui_input_to_command(Ref<Evt> p_event) {
                return reinterpret_cast<Derived*>(this)->from_gui_input_to_command_impl(p_event);
            }

            template<class Derived, class Command, class Evt>
            CommandEmitter<Derived, Command, Evt>::CommandEmitter(UndoRedo* p_undo_redo) : undo_redo(p_undo_redo) {

            }
        }
    }
}

#endif //ISOMETRIC_MAPS_COMMANDEMITTER_H

#endif
