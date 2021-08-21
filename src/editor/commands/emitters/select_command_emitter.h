#ifdef TOOLS_ENABLED

#ifndef ISOMETRIC_MAPS_SELECT_COMMAND_EMITTER_H
#define ISOMETRIC_MAPS_SELECT_COMMAND_EMITTER_H

#include <modules/isometric_maps/src/editor/commands/select_positionable_command.h>
#include "command_emitter.h"

namespace editor {
    namespace commands {
        namespace emitters {
            class SelectCommandEmitter : public CommandEmitter<SelectCommandEmitter, InputEventMouse>{

                friend class CommandEmitter<SelectCommandEmitter, InputEventMouse>;

            public:
                void set_map(node::IsometricMap* p_map);

                SelectCommandEmitter() = delete;
                explicit SelectCommandEmitter(UndoRedo* undo_redo);
                ~SelectCommandEmitter() = default;

            private:
                node::IsometricMap* map;

                Vector<Ref<Command>> from_gui_input_to_command_impl(Ref<InputEventMouse> p_event);
            };
        }
    }
}


#endif //ISOMETRIC_MAPS_SELECT_COMMAND_EMITTER_H

#endif
