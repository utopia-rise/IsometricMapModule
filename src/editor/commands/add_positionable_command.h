#ifndef ISOMETRIC_MAPS_ADD_POSITIONABLE_COMMAND_H
#define ISOMETRIC_MAPS_ADD_POSITIONABLE_COMMAND_H

#ifdef TOOLS_ENABLED

#include "command.h"
#include "node/isometric_map.h"

namespace editor {
    namespace commands {
        class AddPositionableCommand : public Command {
        public:
            void redo() override;
            void undo() override;

            void set_aabb(const AABB& p_aabb);
            void set_positionable_id(int id);

            AddPositionableCommand();
            ~AddPositionableCommand() override = default;

        private:
            AABB aabb;
            int positionable_id;
        };
    }// namespace commands
}// namespace editor

#endif
#endif// ISOMETRIC_MAPS_ADD_POSITIONABLE_COMMAND_H
