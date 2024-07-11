#ifndef ISOMETRIC_MAPS_DRAG_AND_DROP_COMMAND_EMITTER_H
#define ISOMETRIC_MAPS_DRAG_AND_DROP_COMMAND_EMITTER_H

#ifdef TOOLS_ENABLED
#include "command_emitter.h"
#include "editor/commands/command.h"
#include "node/isometric_map.h"

namespace editor {
    namespace commands {
        namespace emitters {
            static constexpr const char drag_and_drop_action_name[]{"Add positionables with drag and drop"};

            class DragAndDropCommandEmitter : public CommandEmitter<DragAndDropCommandEmitter, InputEventMouse, node::IsometricMap, drag_and_drop_action_name> {
                friend class CommandEmitter<DragAndDropCommandEmitter, InputEventMouse, node::IsometricMap, drag_and_drop_action_name>;

            public:
                DragAndDropCommandEmitter();
                ~DragAndDropCommandEmitter();

            private:
                struct Chunk {
                    Vector<node::IsometricPositionable*> current_preview_nodes;
                    Node* preview_node;
                    Vector3i begin_position;
                    Vector3i last_position;
                };

                Vector3 initial_position;
                Vector3 limit_position;

                HashMap<Vector2i, Chunk*> preview_chunks;

                Vector<Ref<Command<node::IsometricMap>>> from_gui_input_to_command_impl([[maybe_unused]] Ref<InputEventMouse> p_event);

                static void _clear_current_preview_nodes(Chunk* p_chunk, int new_size);

                static AABB _calculate_real_aabb(const Vector3& initial_position, const Vector3& limit_position, const Vector3& positionable_size);

                static Vector<Vector3> _calculate_positionables_positions(const Vector3& initial_position, const Vector3& limit_position, const Vector3& positionable_size);

                Vector<Chunk*> _get_chunks_for_position_interval(const Vector3& p_initial_position, const Vector3& p_target_position);
                static Vector2i _get_chunk_position(const Vector3& p_position);
                Chunk* _get_or_create_chunks_for_position(const Vector2i& p_position);
            };
        }// namespace emitters
    }// namespace commands
}// namespace editor

#endif
#endif// ISOMETRIC_MAPS_DRAG_AND_DROP_COMMAND_EMITTER_H