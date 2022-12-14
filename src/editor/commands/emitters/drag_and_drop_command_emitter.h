#ifndef ISOMETRIC_MAPS_DRAG_AND_DROP_COMMAND_EMITTER_H
#define ISOMETRIC_MAPS_DRAG_AND_DROP_COMMAND_EMITTER_H

#ifdef TOOLS_ENABLED

    #include "command_emitter.h"
    #include <modules/isometric_maps/src/editor/commands/command.h>
    #include <modules/isometric_maps/src/node/isometric_map.h>

namespace editor {
    namespace commands {
        namespace emitters {
            class DragAndDropCommandEmitter : public CommandEmitter<DragAndDropCommandEmitter, InputEventMouse> {
                friend class CommandEmitter<DragAndDropCommandEmitter, InputEventMouse>;

            public:
                DragAndDropCommandEmitter() = delete;
                explicit DragAndDropCommandEmitter(UndoRedo* undo_redo);
                ~DragAndDropCommandEmitter();

            private:
                Vector<node::IsometricPositionable*> current_preview_nodes;

                Vector3 initial_position;
                Vector3 limit_position;

                Vector<Ref<Command>> from_gui_input_to_command_impl(Ref<InputEventMouse> p_event);

                void _clear_current_preview_nodes(int new_size);

                static AABB _calculate_real_aabb(const Vector3& initial_position, const Vector3& limit_position, const Vector3& positionable_size);

                static Vector<Vector3> _calculate_positionables_positions(
                    const Vector3& initial_position, const Vector3& limit_position, const Vector3& positionable_size
                );
            };
        }// namespace emitters
    }// namespace commands
}// namespace editor

#endif
#endif// ISOMETRIC_MAPS_DRAG_AND_DROP_COMMAND_EMITTER_H