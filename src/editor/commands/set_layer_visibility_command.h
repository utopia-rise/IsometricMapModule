#ifndef ISOMETRIC_MAPS_SET_LAYER_VISIBILITY_COMMAND_H
#define ISOMETRIC_MAPS_SET_LAYER_VISIBILITY_COMMAND_H

#ifdef TOOLS_ENABLED

#include "command.h"
#include "node/isometric_map.h"

namespace editor {
    namespace commands {
        class SetLayerVisibilityCommand : public Command<node::IsometricMap> {
        public:
            void redo() override;
            void undo() override;

            void set_layer_id(uint32_t p_layer_id);
            void set_visible(bool p_is_visible);

            SetLayerVisibilityCommand();
            ~SetLayerVisibilityCommand() override = default;

        private:
            uint32_t layer_id;
            bool is_visible;
        };
    }
}

#endif

#endif //ISOMETRIC_MAPS_SET_LAYER_VISIBILITY_COMMAND_H
