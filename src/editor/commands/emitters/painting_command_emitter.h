#ifndef ISOMETRIC_MAPS_PAINTING_COMMAND_EMITTER_H
#define ISOMETRIC_MAPS_PAINTING_COMMAND_EMITTER_H

#ifdef TOOLS_ENABLED

    #include "command_emitter.h"
    #include <modules/isometric_maps/src/editor/commands/add_positionable_command.h>
    #include <modules/isometric_maps/src/node/isometric_map.h>

namespace editor {
    namespace commands {
        namespace emitters {
            class PaintingCommandEmitter : public CommandEmitter<PaintingCommandEmitter, InputEventMouse> {
                friend class CommandEmitter<PaintingCommandEmitter, InputEventMouse>;

            public:
                PaintingCommandEmitter() = delete;

                explicit PaintingCommandEmitter(UndoRedo* undo_redo);

                ~PaintingCommandEmitter();

            private:
                node::IsometricPositionable* current_preview_node;

                Vector<Ref<Command>> from_gui_input_to_command_impl(Ref<InputEventMouse> p_event);

                void _clear_current_preview_node();
            };
        }// namespace emitters
    }// namespace commands
}// namespace editor

#endif

#endif// ISOMETRIC_MAPS_PAINTING_COMMAND_EMITTER_H
