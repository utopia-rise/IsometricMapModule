#ifdef TOOLS_ENABLED

#ifndef ISOMETRIC_MAPS_ROTATE_EDITOR_PLANE_COMMAND_H
#define ISOMETRIC_MAPS_ROTATE_EDITOR_PLANE_COMMAND_H


#include "command.h"

namespace editor {
    namespace commands {
        class RotateEditorPlaneCommand : public Command {
        public:
            void redo() override;
            void undo() override;

            void set_new_axis(Vector3::Axis p_axis);

            void set_former_axis(Vector3::Axis p_axis);
            void set_former_position(int p_position);

            RotateEditorPlaneCommand() = default;
            ~RotateEditorPlaneCommand() override = default;

        private:
            static void set_axis_and_position(Vector3::Axis p_axis, int p_position);

            Vector3::Axis new_axis;

            Vector3::Axis former_axis;
            int former_position;
        };
    }
}


#endif //ISOMETRIC_MAPS_ROTATE_EDITOR_PLANE_COMMAND_H

#endif
