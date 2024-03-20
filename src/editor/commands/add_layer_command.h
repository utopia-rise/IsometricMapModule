#ifndef ISOMETRIC_MAPS_ADD_LAYER_COMMAND_H
#define ISOMETRIC_MAPS_ADD_LAYER_COMMAND_H


#ifdef TOOLS_ENABLED

#include "command.h"
#include "node/isometric_map.h"

namespace editor {
    namespace commands {
        class AddLayerCommand : public Command<node::IsometricMap> {
        public:
            void redo() override;
            void undo() override;

            void set_layer_id(uint32_t p_layer_id);
            void set_layer_name(const String& p_layer_name);

            AddLayerCommand();
            ~AddLayerCommand() override = default;

        private:
            uint32_t layer_id;
            String layer_name;
        };
    }
}
#endif


#endif //ISOMETRIC_MAPS_ADD_LAYER_COMMAND_H
