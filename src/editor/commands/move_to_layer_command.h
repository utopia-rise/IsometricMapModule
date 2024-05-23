#ifndef ISOMETRIC_MAPS_MOVE_TO_LAYER_COMMAND_H
#define ISOMETRIC_MAPS_MOVE_TO_LAYER_COMMAND_H

#ifdef TOOLS_ENABLED

#include "command.h"
#include "node/isometric_map.h"

namespace editor {
    namespace commands {
        class MoveToLayerCommand : public Command<node::IsometricMap> {
        public:
            void redo() override;
            void undo() override;

            void set_position(const Vector3& p_position);
            void set_new_layer_id(uint32_t p_new_layer_id);
            void set_former_layer_id(uint32_t p_former_layer_id);

            MoveToLayerCommand() = default;
            ~MoveToLayerCommand() override = default;

        private:
            Vector3 position = Vector3();
            uint32_t new_layer_id = node::IsometricMap::NO_LAYER_ID;
            uint32_t former_layer_id = node::IsometricMap::NO_LAYER_ID;
        };
    }
}

#endif// ISOMETRIC_MAPS_MOVE_TO_LAYER_COMMAND_H

#endif
