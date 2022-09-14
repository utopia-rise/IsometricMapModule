#ifdef TOOLS_ENABLED

#ifndef ISOMETRIC_MAPS_MOVE_EDITOR_VIEW_LIMITER_COMMAND_EMITTER_H
#define ISOMETRIC_MAPS_MOVE_EDITOR_VIEW_LIMITER_COMMAND_EMITTER_H

#include <modules/isometric_maps/src/editor/editor_plane.h>
#include <modules/isometric_maps/src/editor/commands/command.h>
#include "command_emitter.h"
#include <modules/isometric_maps/src/editor/commands/composite_command.h>
#include <modules/isometric_maps/src/node/isometric_map.h>

namespace editor {
    namespace commands {
        namespace emitters {
            class MoveEditorViewLimiterCommandEmitter : public CommandEmitter<MoveEditorViewLimiterCommandEmitter, InputEventMouseButton> {
                friend class CommandEmitter<MoveEditorViewLimiterCommandEmitter, InputEventMouseButton>;

            public:
                MoveEditorViewLimiterCommandEmitter() = delete;
                explicit MoveEditorViewLimiterCommandEmitter(UndoRedo* undo_redo);
                ~MoveEditorViewLimiterCommandEmitter() = default;

            private:
                Vector<Ref<Command>> from_gui_input_to_command_impl(Ref<InputEventMouseButton> p_event);
            };
        }
    }
}

#endif //ISOMETRIC_MAPS_MOVE_EDITOR_VIEW_LIMITER_COMMAND_EMITTER_H

#endif
