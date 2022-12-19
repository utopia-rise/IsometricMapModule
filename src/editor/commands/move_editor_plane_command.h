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

            void set_new_position(int p_new_position);
            void set_old_position(int p_old_position);
            void set_plane_type(EditorPlane::PlaneType p_plane_type);

            MoveEditorPlaneCommand();
            ~MoveEditorPlaneCommand() override = default;

        private:
            EditorPlane::PlaneType plane_type;
            int new_position;
            int old_position;
        };
    }
}


#endif //ISOMETRIC_MAPS_MOVE_EDITION_GRID_COMAND_H

#endif
