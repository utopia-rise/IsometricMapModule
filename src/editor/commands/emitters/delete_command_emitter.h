#ifdef TOOLS_ENABLED

#ifndef ISOMETRIC_MAPS_DELETE_COMMAND_EMITTER_H
#define ISOMETRIC_MAPS_DELETE_COMMAND_EMITTER_H

#include <modules/isometric_maps/src/node/isometric_map.h>
#include <modules/isometric_maps/src/editor/commands/command.h>
#include "command_emitter.h"

namespace editor {
    namespace commands {
        namespace emitters {
            class DeleteCommandEmitter : public CommandEmitter<DeleteCommandEmitter, InputEventKey> {
                friend class CommandEmitter<DeleteCommandEmitter, InputEventKey>;

            public:
                void set_map(node::IsometricMap* p_map);

                DeleteCommandEmitter() = delete;
                explicit DeleteCommandEmitter(UndoRedo* undo_redo);
                ~DeleteCommandEmitter() = default;

            private:
                node::IsometricMap* map;

                Vector<Ref<Command>> from_gui_input_to_command_impl(Ref<InputEventKey> p_event);
            };
        }
    }
}

#endif //ISOMETRIC_MAPS_DELETE_COMMAND_EMITTER_H

#endif
