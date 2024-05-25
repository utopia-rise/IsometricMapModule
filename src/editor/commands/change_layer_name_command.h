#ifndef ISOMETRIC_MAPS_CHANGE_LAYER_NAME_COMMAND_H
#define ISOMETRIC_MAPS_CHANGE_LAYER_NAME_COMMAND_H

#include "command.h"
#include "node/isometric_map.h"
#ifdef TOOLS_ENABLED

namespace editor {
    namespace commands {
        class ChangeLayerNameCommand : public Command<node::IsometricMap> {
        public:
            void redo() override;
            void undo() override;

            void set_layer_id(uint32_t p_layer_id);
            void set_new_layer_name(const String& p_new_layer_name);
            void set_former_layer_name(const String& p_former_layer_name);

            ChangeLayerNameCommand() = default;
            ~ChangeLayerNameCommand() override = default;

        private:
            String new_layer_name;
            String former_layer_name;
            uint32_t layer_id;
        };
    }
}

#endif

#endif// ISOMETRIC_MAPS_CHANGE_LAYER_NAME_COMMAND_H
