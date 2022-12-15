#ifndef ISOMETRIC_MAPS_ROTATE_EDITOR_PLANE_COMMAND_EMITTER_H
#define ISOMETRIC_MAPS_ROTATE_EDITOR_PLANE_COMMAND_EMITTER_H

#ifdef TOOLS_ENABLED
    #include "command_emitter.h"
    #include "editor/commands/command.h"

namespace editor {
    namespace commands {
        namespace emitters {
            class RotateEditorPlaneCommandEmitter : public CommandEmitter<RotateEditorPlaneCommandEmitter, InputEventKey> {
                friend class CommandEmitter<RotateEditorPlaneCommandEmitter, InputEventKey>;

            public:
                RotateEditorPlaneCommandEmitter() = delete;

                explicit RotateEditorPlaneCommandEmitter(UndoRedo* undo_redo);

                ~RotateEditorPlaneCommandEmitter() = default;

            private:
                Vector<Ref<Command>> from_gui_input_to_command_impl(Ref<InputEventKey> p_event);
            };
        }// namespace emitters
    }// namespace commands
}// namespace editor

#endif
#endif// ISOMETRIC_MAPS_ROTATE_EDITOR_PLANE_COMMAND_EMITTER_H
