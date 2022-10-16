#ifdef TOOLS_ENABLED

#ifndef ISOMETRIC_MAPS_SET_POSITIONABLE_VISIBILITY_COMMAND_H
#define ISOMETRIC_MAPS_SET_POSITIONABLE_VISIBILITY_COMMAND_H

#include "command.h"


namespace editor {
    namespace commands {
        class SetPositionableVisibilityCommand : public Command {
        public:
            void redo() override;
            void undo() override;

            void set_position(const Vector3& p_position);

            SetPositionableVisibilityCommand();
            ~SetPositionableVisibilityCommand() override = default;

        private:
            Vector3 position;
        };
    }
}


#endif //ISOMETRIC_MAPS_SET_POSITIONABLE_VISIBILITY_COMMAND_H

#endif