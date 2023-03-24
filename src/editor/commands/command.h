#ifndef ISOMETRIC_MAPS_COMMAND_H
#define ISOMETRIC_MAPS_COMMAND_H

#include "core/object/ref_counted.h"
#ifdef TOOLS_ENABLED


namespace editor {
    namespace commands {

        class Command : public RefCounted {
            GDCLASS(Command, RefCounted)

        protected:
            static void _bind_methods();

        public:
            virtual void redo() = 0;
            virtual void undo() = 0;

            void append_to_undoredo();

            Command() = default;
            ~Command() override = default;
        };
    }// namespace commands
}// namespace editor

#endif
#endif// ISOMETRIC_MAPS_COMMAND_H
