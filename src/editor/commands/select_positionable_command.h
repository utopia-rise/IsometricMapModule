#ifdef TOOLS_ENABLED

#ifndef ISOMETRIC_MAPS_SELECT_POSITIONABLE_COMMAND_H
#define ISOMETRIC_MAPS_SELECT_POSITIONABLE_COMMAND_H

#include <modules/isometric_maps/src/node/isometric_map.h>
#include "command.h"

namespace editor {
    namespace commands {
        class SelectPositionableCommand : public Command {

        public:
            void redo() override;
            void undo() override;

            void set_position(const Vector3& p_position);
            void set_should_deselect_first(bool p_should);

            SelectPositionableCommand();
            ~SelectPositionableCommand() override = default;

        private:
            Vector3 position;
            bool should_deselect_first;

            Vector<Vector3> selected_cache;

        public:
            static void _bind_methods_impl();
        };
    }
}

#endif //ISOMETRIC_MAPS_SELECT_POSITIONABLE_COMMAND_H

#endif