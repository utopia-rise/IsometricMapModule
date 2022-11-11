#ifndef ISOMETRIC_MAPS_SELECT_COMMAND_EMITTER_H
#define ISOMETRIC_MAPS_SELECT_COMMAND_EMITTER_H

#ifdef TOOLS_ENABLED

    #include "command_emitter.h"
    #include <modules/isometric_maps/src/editor/commands/select_positionable_command.h>

namespace editor {
    namespace commands {
        namespace emitters {
            class SelectCommandEmitter : public CommandEmitter<SelectCommandEmitter, InputEventMouse> {
                friend class CommandEmitter<SelectCommandEmitter, InputEventMouse>;

            public:
                SelectCommandEmitter() = delete;

                explicit SelectCommandEmitter(UndoRedo* undo_redo);

                ~SelectCommandEmitter() = default;

            private:
                Vector<Ref<Command>> from_gui_input_to_command_impl(Ref<InputEventMouse> p_event);
            };
        }// namespace emitters
    }// namespace commands
}// namespace editor

#endif
#endif// ISOMETRIC_MAPS_SELECT_COMMAND_EMITTER_H
