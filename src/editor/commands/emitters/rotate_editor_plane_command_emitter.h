#ifndef ISOMETRIC_MAPS_ROTATE_EDITOR_PLANE_COMMAND_EMITTER_H
#define ISOMETRIC_MAPS_ROTATE_EDITOR_PLANE_COMMAND_EMITTER_H

#ifdef TOOLS_ENABLED
#include "command_emitter.h"
#include "core/input/input_event.h"
#include "editor/commands/command.h"
#include "node/isometric_map.h"

namespace editor {
    namespace commands {
        namespace emitters {
            static constexpr const char rotate_editor_plane_action_name[]{"Rotate editor plane"};

            class RotateEditorPlaneCommandEmitter : public CommandEmitter<RotateEditorPlaneCommandEmitter, InputEventKey, node::IsometricMap, rotate_editor_plane_action_name> {
                friend class CommandEmitter<RotateEditorPlaneCommandEmitter, InputEventKey, node::IsometricMap, rotate_editor_plane_action_name>;

            public:
                RotateEditorPlaneCommandEmitter() = default;
                ~RotateEditorPlaneCommandEmitter() = default;

            private:
                Vector<Ref<Command<node::IsometricMap>>> from_gui_input_to_command_impl(Ref<InputEventKey> p_event);
            };
        }// namespace emitters
    }// namespace commands
}// namespace editor

#endif
#endif// ISOMETRIC_MAPS_ROTATE_EDITOR_PLANE_COMMAND_EMITTER_H
