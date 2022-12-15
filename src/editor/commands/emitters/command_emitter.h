#ifndef ISOMETRIC_MAPS_COMMAND_EMITTER_H
#define ISOMETRIC_MAPS_COMMAND_EMITTER_H

#ifdef TOOLS_ENABLED
    #include "../command.h"

    #include <core/os/input_event.h>
    #include <core/undo_redo.h>

namespace editor {
    namespace commands {
        namespace emitters {

            template<class Derived, class Evt>
            class CommandEmitter {
            public:
                void on_gui_input(const Ref<InputEvent>& p_event);

                CommandEmitter() = delete;
                explicit CommandEmitter(UndoRedo* p_undo_redo);
                ~CommandEmitter() = default;

            private:
                UndoRedo* undo_redo;
                Vector<Ref<Command>> from_gui_input_to_command(Ref<Evt> p_event);
            };

            template<class Derived, class Evt>
            void CommandEmitter<Derived, Evt>::on_gui_input(const Ref<InputEvent>& p_event) {
                Ref<Evt> event {p_event};
                if (!event.is_null()) {
                    Vector<Ref<Command>> commands {from_gui_input_to_command(event)};

                    bool has_valid_command {false};
                    for (int i = 0; i < commands.size(); ++i) {
                        Ref<Command> command {commands[i]};
                        if (!command.is_valid()) { continue; }
                        if (!has_valid_command) { undo_redo->create_action(); }
                        has_valid_command = true;
                        command->append_to_undoredo(undo_redo);
                    }

                    if (has_valid_command) { undo_redo->commit_action(); }
                }
            }

            template<class Derived, class Evt>
            Vector<Ref<Command>> CommandEmitter<Derived, Evt>::from_gui_input_to_command(Ref<Evt> p_event) {
                return reinterpret_cast<Derived*>(this)->from_gui_input_to_command_impl(p_event);
            }

            template<class Derived, class Evt>
            CommandEmitter<Derived, Evt>::CommandEmitter(UndoRedo* p_undo_redo) : undo_redo(p_undo_redo) {}
        }// namespace emitters
    }// namespace commands
}// namespace editor

#endif
#endif// ISOMETRIC_MAPS_COMMAND_EMITTER_H
