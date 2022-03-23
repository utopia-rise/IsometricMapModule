#ifdef TOOLS_ENABLED

#ifndef ISOMETRIC_MAPS_SELECT_ALL_COMMAND_EMITTER_H
#define ISOMETRIC_MAPS_SELECT_ALL_COMMAND_EMITTER_H

#include <modules/isometric_maps/src/editor/commands/command.h>
#include "command_emitter.h"

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
        }
    }
}


#endif //ISOMETRIC_MAPS_SELECT_ALL_COMMAND_EMITTER_H

#endif
