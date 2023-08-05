#ifndef ISOMETRIC_MAPS_MOVE_EDITOR_VIEW_LIMITER_COMMAND_EMITTER_H
#define ISOMETRIC_MAPS_MOVE_EDITOR_VIEW_LIMITER_COMMAND_EMITTER_H

#ifdef TOOLS_ENABLED
#include "command_emitter.h"
#include "editor/commands/command.h"
#include "editor/commands/composite_command.h"
#include "editor/editor_plane.h"
#include "node/isometric_map.h"

namespace editor {
    namespace commands {
        namespace emitters {
            typedef
#ifdef OSX_ENABLED
              InputEventPanGesture
#else
              InputEventMouseButton
#endif
                ScrollInputEvent;

            static constexpr const char move_view_limiter_action_name[]{"Move view limiter"};

            class MoveEditorViewLimiterCommandEmitter : public CommandEmitter<MoveEditorViewLimiterCommandEmitter, ScrollInputEvent, move_view_limiter_action_name> {
                friend class CommandEmitter<MoveEditorViewLimiterCommandEmitter, ScrollInputEvent, move_view_limiter_action_name>;

            public:
                MoveEditorViewLimiterCommandEmitter();
                ~MoveEditorViewLimiterCommandEmitter() = default;

            private:
                enum EventMotion {
                    FORWARD,
                    BACKWARD,
                    NONE
                };

#ifdef OSX_ENABLED
                real_t cumulative_scroll_delta;
                uint64_t cumulative_scroll_last_time;
#endif

                Vector<Ref<Command>> from_gui_input_to_command_impl(Ref<ScrollInputEvent> p_event);

                static bool _is_event_activated(Ref<ScrollInputEvent> p_event);

                EventMotion _is_event_forward(Ref<ScrollInputEvent> p_event);
            };
        }// namespace emitters
    }// namespace commands
}// namespace editor

#endif

#endif// ISOMETRIC_MAPS_MOVE_EDITOR_VIEW_LIMITER_COMMAND_EMITTER_H
