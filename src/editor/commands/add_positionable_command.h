#ifdef TOOLS_ENABLED

#ifndef ISOMETRIC_MAPS_ADD_POSITIONABLE_COMMAND_H
#define ISOMETRIC_MAPS_ADD_POSITIONABLE_COMMAND_H

#include <modules/isometric_maps/src/node/isometric_map.h>
#include "command.h"


namespace editor {
    namespace commands {
        class AddPositionableCommand : public Command {

        public:
            void redo() override;
            void undo() override;

            void set_aabb(const AABB& p_aabb);
            void set_positionable_id(int id);
            void set_map(node::IsometricMap* p_map);

            AddPositionableCommand();
            ~AddPositionableCommand() override = default;

        private:
            AABB aabb;
            int positionable_id;
            node::IsometricMap* map;

        public:
            static void _bind_methods_impl();

        };
    }
}


#endif //ISOMETRIC_MAPS_ADD_POSITIONABLE_COMMAND_H

#endif