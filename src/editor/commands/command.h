#ifdef TOOLS_ENABLED

#ifndef ISOMETRIC_MAPS_COMMAND_H
#define ISOMETRIC_MAPS_COMMAND_H

#include <core/class_db.h>
#include <core/reference.h>
#include <core/undo_redo.h>


namespace editor {
    namespace commands {

        template<class Derived>
        class Command : public Reference{

        protected:
            static void _bind_methods() {
                ClassDB::bind_method(D_METHOD("redo"), &Derived::redo);
                ClassDB::bind_method(D_METHOD("undo"), &Derived::undo);

                Derived::_bind_methods_impl();
            }

        public:
            void redo();
            void undo();

            void append_to_undoredo(UndoRedo* undo_redo);

            Command() = default;
            ~Command() override = default;
        };

        template<class Derived>
        void Command<Derived>::redo() {
            reinterpret_cast<Derived*>(this)->redo_implementation();
        }

        template<class Derived>
        void Command<Derived>::undo() {
            reinterpret_cast<Derived*>(this)->undo_implementation();
        }

        template<class Derived>
        void Command<Derived>::append_to_undoredo(UndoRedo* undo_redo) {
            undo_redo->add_do_method(this, redo());
            undo_redo->add_undo_method(this, undo());
        }
    }
}

#endif //ISOMETRIC_MAPS_COMMAND_H

#endif
