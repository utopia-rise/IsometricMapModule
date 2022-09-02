#ifdef TOOLS_ENABLED

#ifndef ISOMETRIC_MAPS_ROTATE_EDITOR_PLANE_COMMAND_EMITTER_H
#define ISOMETRIC_MAPS_ROTATE_EDITOR_PLANE_COMMAND_EMITTER_H


#include <modules/isometric_maps/src/editor/commands/command.h>
#include "command_emitter.h"

namespace editor {
    namespace commands {
        namespace emitters {
            class RotateEditorPlaneCommandEmitter : public CommandEmitter<RotateEditorPlaneCommandEmitter, InputEventKey> {
                friend class CommandEmitter<RotateEditorPlaneCommandEmitter, InputEventKey>;

            public:
                RotateEditorPlaneCommandEmitter() = delete;
                explicit RotateEditorPlaneCommandEmitter(UndoRedo* undo_redo);
                ~RotateEditorPlaneCommandEmitter() = default;

            private:
                Vector<Ref<Command>> from_gui_input_to_command_impl(Ref<InputEventKey> p_event);
            };
        }
    }
}


#endif //ISOMETRIC_MAPS_ROTATE_EDITOR_PLANE_COMMAND_EMITTER_H

#endif