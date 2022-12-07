#ifdef TOOLS_ENABLED

#ifndef ISOMETRIC_MAPS_MOVE_EDITION_GRID_COMAND_H
#define ISOMETRIC_MAPS_MOVE_EDITION_GRID_COMAND_H


#include <modules/isometric_maps/src/editor/editor_plane.h>
#include "command.h"

namespace editor {
    namespace commands {
        class MoveEditorPlaneCommand : public Command {
        public:
            void redo() override;
            void undo() override;

            void set_is_forward(bool p_is_forward);
            void set_plane_type(EditorPlane::PlaneType p_plane_type);
            void set_should_set_inf_on_max(bool p_should_set_inf_on_max);

            MoveEditorPlaneCommand();
            ~MoveEditorPlaneCommand() override = default;

        private:
            bool is_forward;
            EditorPlane::PlaneType plane_type;
            bool should_set_inf_on_max;
            int former_position;
        };
    }
}


#endif //ISOMETRIC_MAPS_MOVE_EDITION_GRID_COMAND_H

#endif
