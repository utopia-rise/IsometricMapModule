#ifdef TOOLS_ENABLED

#ifndef ISOMETRIC_MAPS_PAINTING_COMMAND_EMITTER_H
#define ISOMETRIC_MAPS_PAINTING_COMMAND_EMITTER_H

#include <modules/isometric_maps/src/editor/commands/add_positionable_command.h>
#include <modules/isometric_maps/src/node/isometric_map.h>
#include "command_emitter.h"

namespace editor {
    namespace commands {
        namespace emitters {
            class PaintingCommandEmitter : public CommandEmitter<PaintingCommandEmitter, AddPositionableCommand, InputEventMouseMotion>{

                friend class CommandEmitter<PaintingCommandEmitter, AddPositionableCommand, InputEventMouseMotion>;

            public:
                void set_map(node::IsometricMap* p_map);

                PaintingCommandEmitter() = delete;
                explicit PaintingCommandEmitter(UndoRedo* undo_redo);
                ~PaintingCommandEmitter() = default;

            private:
                node::IsometricMap* map;
                Vector3 last_event_position;
                bool was_last_event_intercepted;

                Vector<Ref<editor::commands::AddPositionableCommand>>
                from_gui_input_to_command_impl(Ref<InputEventMouseMotion> p_event);

            };
        }
    }
}


#endif //ISOMETRIC_MAPS_PAINTING_COMMAND_EMITTER_H

#endif
