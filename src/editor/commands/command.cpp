#ifdef TOOLS_ENABLED

#include "command.h"



using namespace editor::commands;

void Command::append_to_undoredo() {
    EditorUndoRedoManager::get_singleton()->add_do_method(this, "redo");
    EditorUndoRedoManager::get_singleton()->add_undo_method(this, "undo");
}

void Command::_bind_methods() {
    ClassDB::bind_method(D_METHOD("redo"), &Command::redo);
    ClassDB::bind_method(D_METHOD("undo"), &Command::undo);
}

#endif