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

            MoveEditorPlaneCommand();
            ~MoveEditorPlaneCommand() override = default;

        private:
            static void act(bool p_is_forward);

            bool is_forward;
        };
    }
}


#endif //ISOMETRIC_MAPS_MOVE_EDITION_GRID_COMAND_H

#endif
