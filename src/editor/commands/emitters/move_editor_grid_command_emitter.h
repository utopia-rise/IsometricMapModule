#ifndef ISOMETRIC_MAPS_MOVE_EDITION_GRID_COMMAND_EMITTER_H
#define ISOMETRIC_MAPS_MOVE_EDITION_GRID_COMMAND_EMITTER_H

#ifdef TOOLS_ENABLED
#include "command_emitter.h"
#include "core/input/input_event.h"
#include "editor/commands/command.h"

namespace editor {
    namespace commands {
        namespace emitters {
            class MoveEditorGridCommandEmitter : public CommandEmitter<MoveEditorGridCommandEmitter, InputEventKey> {
                friend class CommandEmitter<MoveEditorGridCommandEmitter, InputEventKey>;

            public:
                MoveEditorGridCommandEmitter() = default;
                ~MoveEditorGridCommandEmitter() = default;

            private:
                Vector<Ref<Command>> from_gui_input_to_command_impl(Ref<InputEventKey> p_event);
            };
        }// namespace emitters
    }// namespace commands
}// namespace editor

#endif
#endif// ISOMETRIC_MAPS_MOVE_EDITION_GRID_COMMAND_EMITTER_H