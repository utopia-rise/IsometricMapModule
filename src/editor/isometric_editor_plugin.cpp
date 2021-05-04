#include "isometric_editor_plugin.h"
#include "../node/isometric_map.h"

using namespace editor;

IsometricEditorPlugin::IsometricEditorPlugin(): selected_map{nullptr}, current_mode{Mode::NONE} {

}

IsometricEditorPlugin* IsometricEditorPlugin::get_instance() {
    static IsometricEditorPlugin* instance{memnew(IsometricEditorPlugin)};
    return instance;
}

void IsometricEditorPlugin::_notificationv(int p_notification, bool p_reversed) {
    EditorPlugin::_notificationv(p_notification, p_reversed);
}

void IsometricEditorPlugin::edit(Object* p_object) {
    selected_map = cast_to<node::IsometricMap>(p_object);
}

bool IsometricEditorPlugin::handles(Object* p_object) const {
    return cast_to<node::IsometricMap>(p_object);
}

void IsometricEditorPlugin::clear() {
    selected_map = nullptr;
}

bool IsometricEditorPlugin::forward_canvas_gui_input(const Ref<InputEvent> &p_event) {
    return false;
}


void IsometricEditorPlugin::_bind_methods() {
}



