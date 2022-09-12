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

            MoveEditorPlaneCommand();
            ~MoveEditorPlaneCommand() override = default;

        private:
            void act(bool p_is_forward);

            bool is_forward;
            EditorPlane::PlaneType plane_type;
        };
    }
}


#endif //ISOMETRIC_MAPS_MOVE_EDITION_GRID_COMAND_H

#endif
