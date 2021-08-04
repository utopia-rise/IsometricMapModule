#include "painting_command_emitter.h"

using namespace editor::commands::emitters;

Vector<Ref<editor::commands::AddPositionableCommand>> PaintingCommandEmitter::from_gui_input_to_command_impl(
        const InputEventMouseMotion* p_event) {
    print_line("Event forwarded to PaintingCommandEmitter");
    Vector<Ref<editor::commands::AddPositionableCommand>> commands;
    return commands;
}

PaintingCommandEmitter::PaintingCommandEmitter(UndoRedo* undo_redo) : CommandEmitter(undo_redo) {

}
