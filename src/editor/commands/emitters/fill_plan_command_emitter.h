#ifndef ISOMETRIC_MAPS_FILL_PLAN_COMMAND_EMITTER_H
#define ISOMETRIC_MAPS_FILL_PLAN_COMMAND_EMITTER_H

#ifdef TOOLS_ENABLED

#include "command_emitter.h"
#include "node/isometric_map.h"

#include <core/input/input_event.h>

namespace editor {
    namespace commands {
        namespace emitters {
            static constexpr const char fill_plan_action_name[]{"Fill plan with elements"};

            class FillPlanCommandEmitter : public CommandEmitter<FillPlanCommandEmitter, InputEventMouse, node::IsometricMap, fill_plan_action_name> {
                friend class CommandEmitter<FillPlanCommandEmitter, InputEventMouse, node::IsometricMap, fill_plan_action_name>;

            public:
                FillPlanCommandEmitter() = default;
                ~FillPlanCommandEmitter() = default;

            private:
                Vector<Ref<Command<node::IsometricMap>>> from_gui_input_to_command_impl([[maybe_unused]] Ref<InputEventMouse> p_event);
            };
        }
    }
}

#endif

#endif// ISOMETRIC_MAPS_FILL_PLAN_COMMAND_EMITTER_H
