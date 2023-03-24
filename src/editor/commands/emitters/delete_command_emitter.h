#ifndef ISOMETRIC_MAPS_DELETE_COMMAND_EMITTER_H
#define ISOMETRIC_MAPS_DELETE_COMMAND_EMITTER_H

#ifdef TOOLS_ENABLED
#include "command_emitter.h"
#include "editor/commands/command.h"
#include "node/isometric_map.h"

namespace editor {
    namespace commands {
        namespace emitters {
            class DeleteCommandEmitter : public CommandEmitter<DeleteCommandEmitter, InputEventKey> {
                friend class CommandEmitter<DeleteCommandEmitter, InputEventKey>;

            public:
                DeleteCommandEmitter() = default;
                ~DeleteCommandEmitter() = default;

            private:
                Vector<Ref<Command>> from_gui_input_to_command_impl(Ref<InputEventKey> p_event);
            };
        }// namespace emitters
    }// namespace commands
}// namespace editor

#endif
#endif// ISOMETRIC_MAPS_DELETE_COMMAND_EMITTER_H
