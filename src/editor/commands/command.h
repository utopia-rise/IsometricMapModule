#ifndef ISOMETRIC_MAPS_COMMAND_H
#define ISOMETRIC_MAPS_COMMAND_H


#ifdef TOOLS_ENABLED

#include "core/object/ref_counted.h"
#include "editor/editor_undo_redo_manager.h"
#include "scene/main/node.h"

namespace editor {
    namespace commands {

        template<class TContextNode>
        class Command : public RefCounted {
            GDCLASS(Command<TContextNode>, RefCounted)

        protected:
            TContextNode* context_node;

            static void _bind_methods();

        public:
            virtual void redo() = 0;
            virtual void undo() = 0;
            virtual void set_context_node(TContextNode* p_context_node);

            void append_to_undoredo();

            Command();
            ~Command() override = default;
        };

        template<class TContextNode>
        void Command<TContextNode>::append_to_undoredo() {
            EditorUndoRedoManager::get_singleton()->add_do_method(this, "redo");
            EditorUndoRedoManager::get_singleton()->add_undo_method(this, "undo");
        }

        template<class TContextNode>
        void Command<TContextNode>::set_context_node(TContextNode* p_context_node) {
            context_node = p_context_node;
        }

        template<class TContextNode>
        Command<TContextNode>::Command() : context_node(nullptr) {}

        template<class TContextNode>
        void Command<TContextNode>::_bind_methods() {
            ClassDB::bind_method(D_METHOD("redo"), &Command<TContextNode>::redo);
            ClassDB::bind_method(D_METHOD("undo"), &Command<TContextNode>::undo);
        }
    }// namespace commands
}// namespace editor

#endif
#endif// ISOMETRIC_MAPS_COMMAND_H
