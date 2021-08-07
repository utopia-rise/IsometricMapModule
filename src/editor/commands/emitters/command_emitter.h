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
                bool _on_gui_input(const Ref<InputEvent>& p_event);

                CommandEmitter() = delete;
                explicit CommandEmitter(UndoRedo* p_undo_redo);
                ~CommandEmitter() = default;

            private:
                UndoRedo* undo_redo;

                bool from_gui_input_to_command(Ref<Evt> p_event, Vector<Ref<Command>>& r_ret);
            };

            template<class Derived, class Command, class Evt>
            bool CommandEmitter<Derived, Command, Evt>::_on_gui_input(const Ref<InputEvent>& p_event) {
                Ref<Evt> event{p_event};
                if (!event.is_null()) {
                    Vector<Ref<Command>> commands;
                    bool is_input_intercepted{from_gui_input_to_command(event, commands)};

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

                    return is_input_intercepted;
                }
                return false;
            }

            template<class Derived, class Command, class Evt>
            bool CommandEmitter<Derived, Command, Evt>::from_gui_input_to_command(Ref<Evt> p_event,
                                                                                  Vector<Ref<Command>>& r_ret) {
                return reinterpret_cast<Derived*>(this)->from_gui_input_to_command_impl(p_event, r_ret);
            }

            template<class Derived, class Command, class Evt>
            CommandEmitter<Derived, Command, Evt>::CommandEmitter(UndoRedo* p_undo_redo) : undo_redo(p_undo_redo) {

            }
        }
    }
}

#endif //ISOMETRIC_MAPS_COMMANDEMITTER_H

#endif
