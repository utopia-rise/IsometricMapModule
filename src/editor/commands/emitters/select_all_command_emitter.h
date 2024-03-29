#ifndef ISOMETRIC_MAPS_SELECT_ALL_COMMAND_EMITTER_H
#define ISOMETRIC_MAPS_SELECT_ALL_COMMAND_EMITTER_H

#ifdef TOOLS_ENABLED
#include "command_emitter.h"
#include "core/input/input_event.h"
#include "editor/commands/command.h"
#include "node/isometric_map.h"

namespace editor {
    namespace commands {
        namespace emitters {
            static constexpr const char select_all_action_name[]{"Select all isometric elements"};

            class SelectAllCommandEmitter : public CommandEmitter<SelectAllCommandEmitter, InputEventKey, node::IsometricMap, select_all_action_name> {
                friend class CommandEmitter<SelectAllCommandEmitter, InputEventKey, node::IsometricMap, select_all_action_name>;

            public:
                SelectAllCommandEmitter() = default;
                ~SelectAllCommandEmitter() = default;

            private:
                Vector<Ref<Command<node::IsometricMap>>> from_gui_input_to_command_impl(Ref<InputEventKey> p_event);
            };
        }// namespace emitters
    }// namespace commands
}// namespace editor

#endif
#endif// ISOMETRIC_MAPS_SELECT_ALL_COMMAND_EMITTER_H
