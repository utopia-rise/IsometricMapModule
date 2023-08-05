#ifndef ISOMETRIC_MAPS_PAINTING_COMMAND_EMITTER_H
#define ISOMETRIC_MAPS_PAINTING_COMMAND_EMITTER_H

#ifdef TOOLS_ENABLED
#include "command_emitter.h"
#include "editor/commands/add_positionable_command.h"
#include "node/isometric_map.h"

namespace editor {
    namespace commands {
        namespace emitters {
            static constexpr const char painting_action_name[]{"Paint isometric elements"};

            class PaintingCommandEmitter : public CommandEmitter<PaintingCommandEmitter, InputEventMouse, painting_action_name> {
                friend class CommandEmitter<PaintingCommandEmitter, InputEventMouse, painting_action_name>;

            public:
                PaintingCommandEmitter();
                ~PaintingCommandEmitter();

            private:
                node::IsometricPositionable* current_preview_node;

                Vector<Ref<Command>> from_gui_input_to_command_impl([[maybe_unused]] Ref<InputEventMouse> p_event);

                void _clear_current_preview_node();
            };
        }// namespace emitters
    }// namespace commands
}// namespace editor

#endif

#endif// ISOMETRIC_MAPS_PAINTING_COMMAND_EMITTER_H
