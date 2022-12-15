#ifndef ISOMETRIC_MAPS_SELECT_ALL_COMMAND_EMITTER_H
#define ISOMETRIC_MAPS_SELECT_ALL_COMMAND_EMITTER_H

#ifdef TOOLS_ENABLED
    #include "command_emitter.h"
    #include "editor/commands/command.h"

namespace editor {
    namespace commands {
        namespace emitters {
            class SelectAllCommandEmitter : public CommandEmitter<SelectAllCommandEmitter, InputEventKey> {
                friend class CommandEmitter<SelectAllCommandEmitter, InputEventKey>;

            public:
                SelectAllCommandEmitter() = delete;

                explicit SelectAllCommandEmitter(UndoRedo* undo_redo);

                ~SelectAllCommandEmitter() = default;

            private:
                Vector<Ref<Command>> from_gui_input_to_command_impl(Ref<InputEventKey> p_event);
            };
        }// namespace emitters
    }// namespace commands
}// namespace editor

#endif
#endif// ISOMETRIC_MAPS_SELECT_ALL_COMMAND_EMITTER_H
