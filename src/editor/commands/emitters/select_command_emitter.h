#ifndef ISOMETRIC_MAPS_SELECT_COMMAND_EMITTER_H
#define ISOMETRIC_MAPS_SELECT_COMMAND_EMITTER_H

#ifdef TOOLS_ENABLED
#include "command_emitter.h"
#include "editor/commands/select_positionable_command.h"

namespace editor {
    namespace commands {
        namespace emitters {
            static constexpr const char select_action_name[]{"Select isometric element"};

            class SelectCommandEmitter : public CommandEmitter<SelectCommandEmitter, InputEventMouse, select_action_name> {
                friend class CommandEmitter<SelectCommandEmitter, InputEventMouse, select_action_name>;

            public:
                SelectCommandEmitter() = default;
                ~SelectCommandEmitter() = default;

            private:
                Vector<Ref<Command>> from_gui_input_to_command_impl(Ref<InputEventMouse> p_event);
            };
        }// namespace emitters
    }// namespace commands
}// namespace editor

#endif
#endif// ISOMETRIC_MAPS_SELECT_COMMAND_EMITTER_H
