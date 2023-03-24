#ifdef TOOLS_ENABLED

#include "command.h"

#include "editor/editor_undo_redo_manager.h"

using namespace editor::commands;

void Command::append_to_undoredo() {
    EditorUndoRedoManager* undo_redo = EditorUndoRedoManager::get_singleton();
    undo_redo->add_do_method(this, "redo");
    undo_redo->add_undo_method(this, "undo");
}

void Command::_bind_methods() {
    ClassDB::bind_method(D_METHOD("redo"), &Command::redo);
    ClassDB::bind_method(D_METHOD("undo"), &Command::undo);
}

#endif