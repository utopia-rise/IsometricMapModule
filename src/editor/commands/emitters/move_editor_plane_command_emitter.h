#ifdef TOOLS_ENABLED

#ifndef ISOMETRIC_MAPS_MOVE_EDITION_GRID_COMMAND_EMITTER_H
#define ISOMETRIC_MAPS_MOVE_EDITION_GRID_COMMAND_EMITTER_H

#include <modules/isometric_maps/src/editor/commands/command.h>
#include "command_emitter.h"

namespace editor {
    namespace commands {
        namespace emitters {
            class MoveEditorPlaneCommandEmitter : public CommandEmitter<MoveEditorPlaneCommandEmitter, InputEventKey>{
                friend class CommandEmitter<MoveEditorPlaneCommandEmitter, InputEventKey>;

            public:
                MoveEditorPlaneCommandEmitter() = delete;
                explicit MoveEditorPlaneCommandEmitter(UndoRedo* undo_redo);
                ~MoveEditorPlaneCommandEmitter() = default;

            private:
                Vector<Ref<Command>> from_gui_input_to_command_impl(Ref<InputEventKey> p_event);
            };
        }
    }
}


#endif //ISOMETRIC_MAPS_MOVE_EDITION_GRID_COMMAND_EMITTER_H

#endif