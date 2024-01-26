#ifndef ISOMETRIC_MAPS_MOVE_EDITION_GRID_COMMAND_H
#define ISOMETRIC_MAPS_MOVE_EDITION_GRID_COMMAND_H

#ifdef TOOLS_ENABLED

#include "command.h"
#include "editor/editor_plane.h"
#include "node/isometric_map.h"

namespace editor {
    namespace commands {
        class MoveEditorPlaneCommand : public Command<node::IsometricMap> {
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
    }// namespace commands
}// namespace editor

#endif
#endif// ISOMETRIC_MAPS_MOVE_EDITION_GRID_COMMAND_H
