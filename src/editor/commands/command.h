#ifndef ISOMETRIC_MAPS_COMMAND_H
#define ISOMETRIC_MAPS_COMMAND_H

#ifdef TOOLS_ENABLED

    #include <core/class_db.h>
    #include <core/reference.h>
    #include <core/resource.h>
    #include <core/undo_redo.h>

namespace editor {
    namespace commands {

        class Command : public Resource {// Forced to set as Resource because Undo redo is not expecting a Reference in
            // current godot version, this is will be fixed in 4.0 version of Godot.
            GDCLASS(Command, Resource)

        protected:
            static void _bind_methods();

        public:
            virtual void redo() = 0;
            virtual void undo() = 0;

            void append_to_undoredo(UndoRedo* undo_redo);

            Command() = default;
            ~Command() override = default;
        };
    }// namespace commands
}// namespace editor

#endif
#endif// ISOMETRIC_MAPS_COMMAND_H
