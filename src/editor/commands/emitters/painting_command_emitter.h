#ifdef TOOLS_ENABLED

#ifndef ISOMETRIC_MAPS_PAINTING_COMMAND_EMITTER_H
#define ISOMETRIC_MAPS_PAINTING_COMMAND_EMITTER_H

#include <modules/isometric_maps/src/editor/commands/add_positionable_command.h>
#include <modules/isometric_maps/src/node/isometric_map.h>
#include "command_emitter.h"

namespace editor {
    namespace commands {
        namespace emitters {
            class PaintingCommandEmitter : public CommandEmitter<PaintingCommandEmitter, AddPositionableCommand, InputEventMouse>{

                friend class CommandEmitter<PaintingCommandEmitter, AddPositionableCommand, InputEventMouse>;

            public:
                void set_map(node::IsometricMap* p_map);

                PaintingCommandEmitter() = delete;
                explicit PaintingCommandEmitter(UndoRedo* undo_redo);
                ~PaintingCommandEmitter();

            private:
                node::IsometricMap* map;
                node::IsometricPositionable* current_preview_node;

                Vector<Ref<editor::commands::AddPositionableCommand>>
                from_gui_input_to_command_impl(Ref<InputEventMouse> p_event);

            };
        }
    }
}


#endif //ISOMETRIC_MAPS_PAINTING_COMMAND_EMITTER_H

#endif