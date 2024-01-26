#ifndef ISOMETRIC_MAPS_MOVE_EDITION_GRID_COMMAND_EMITTER_H
#define ISOMETRIC_MAPS_MOVE_EDITION_GRID_COMMAND_EMITTER_H

#ifdef TOOLS_ENABLED
#include "command_emitter.h"
#include "core/input/input_event.h"
#include "editor/commands/command.h"
#include "node/isometric_map.h"

namespace editor {
    namespace commands {
        namespace emitters {
            static constexpr const char move_editor_grid_action_name[]{"Move editor grid"};

            class MoveEditorGridCommandEmitter : public CommandEmitter<MoveEditorGridCommandEmitter, InputEventKey, node::IsometricMap, move_editor_grid_action_name> {
                friend class CommandEmitter<MoveEditorGridCommandEmitter, InputEventKey, node::IsometricMap, move_editor_grid_action_name>;

            public:
                MoveEditorGridCommandEmitter() = default;
                ~MoveEditorGridCommandEmitter() = default;

            private:
                Vector<Ref<Command<node::IsometricMap>>> from_gui_input_to_command_impl(Ref<InputEventKey> p_event);
            };
        }// namespace emitters
    }// namespace commands
}// namespace editor

#endif
#endif// ISOMETRIC_MAPS_MOVE_EDITION_GRID_COMMAND_EMITTER_H