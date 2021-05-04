#include "isometric_editor_plugin.h"

using namespace editor;

IsometricEditorPlugin::IsometricEditorPlugin() {

}

IsometricEditorPlugin* IsometricEditorPlugin::get_instance() {
    static IsometricEditorPlugin* instance{memnew(IsometricEditorPlugin)};
    return instance;
}

void IsometricEditorPlugin::_bind_methods() {
}

void IsometricEditorPlugin::_notificationv(int p_notification, bool p_reversed) {
    EditorPlugin::_notificationv(p_notification, p_reversed);
}


