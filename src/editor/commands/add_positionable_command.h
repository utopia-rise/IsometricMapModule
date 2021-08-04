#ifdef TOOLS_ENABLED

#ifndef ISOMETRIC_MAPS_ADD_POSITIONABLE_COMMAND_H
#define ISOMETRIC_MAPS_ADD_POSITIONABLE_COMMAND_H

#include <modules/isometric_maps/src/node/isometric_map.h>
#include "command.h"


namespace editor {
    namespace commands {
        class AddPositionableCommand : public Command<AddPositionableCommand> {
            GDCLASS(AddPositionableCommand, Reference)

            friend class Command<AddPositionableCommand>;

        public:
            void set_position(const Vector3& p_position);
            void set_positionable_id(int id);
            void set_map(node::IsometricMap* p_map);

            AddPositionableCommand() = default;
            ~AddPositionableCommand() override = default;

        private:
            Vector3 position;
            int positionable_id;
            node::IsometricMap* map;

            void redo_implementation();
            void undo_implementation();

        public:
            static void _bind_methods_impl();

        };
    }
}


#endif //ISOMETRIC_MAPS_ADD_POSITIONABLE_COMMAND_H

#endif