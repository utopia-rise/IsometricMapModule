#ifndef ISOMETRIC_MAPS_MOVE_SELECTION_COMMAND_EMITTER_H
#define ISOMETRIC_MAPS_MOVE_SELECTION_COMMAND_EMITTER_H

#ifdef TOOLS_ENABLED

#include "command_emitter.h"
#include "node/isometric_map.h"

#include <core/input/input_event.h>

namespace editor {
    namespace commands {
        namespace emitters {
            static constexpr const char move_selection_action_name[]{"Move positionables"};

            class MoveSelectionCommandEmitter : public CommandEmitter<MoveSelectionCommandEmitter, InputEventMouse, node::IsometricMap, move_selection_action_name, UndoRedo::MERGE_DISABLE, true> {
                friend class CommandEmitter<MoveSelectionCommandEmitter, InputEventMouse, node::IsometricMap, move_selection_action_name, UndoRedo::MERGE_DISABLE, true>;

            private:
                Vector<Ref<Command<node::IsometricMap>>> from_gui_input_to_command_impl([[maybe_unused]] Ref<InputEventMouse> p_event);

                HashMap<Vector3, node::IsometricPositionable*> current_preview_nodes;
                HashMap<Vector3, uint32_t> initial_layer_ids;
                Vector3 initial_mouse_position {-1, -1, -1};
                Vector3 new_mouse_position {-1, -1, -1};
                bool is_move_valid = true;

            public:
                MoveSelectionCommandEmitter() = default;
                ~MoveSelectionCommandEmitter() = default;
            };
        }
    }
}

#endif

#endif// ISOMETRIC_MAPS_MOVE_SELECTION_COMMAND_EMITTER_H
