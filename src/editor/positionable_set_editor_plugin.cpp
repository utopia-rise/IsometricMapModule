#ifdef TOOLS_ENABLED

    #include "positionable_set_editor_plugin.h"
    #include "resource/positionable_set.h"

using namespace editor;

const char* POSITIONABLE_SET_EDITOR_BUTTON_TITLE {"Edit Positionable Set"};

PositionableSetEditorPlugin* PositionableSetEditorPlugin::get_instance() {
    static PositionableSetEditorPlugin* instance {memnew(PositionableSetEditorPlugin)};
    return instance;
}

void PositionableSetEditorPlugin::_notification(int p_notification) {
    if (p_notification == NOTIFICATION_READY) {
        positionable_set_editor = memnew(inspector::PositionableSetEditor);
        positionable_set_editor_button = add_control_to_bottom_panel(positionable_set_editor, POSITIONABLE_SET_EDITOR_BUTTON_TITLE);

        positionable_set_editor_button->set_visible(false);
    }
}

bool PositionableSetEditorPlugin::handles(Object* p_object) const {
    return Object::cast_to<resource::PositionableSet>(p_object);
}

void PositionableSetEditorPlugin::make_visible(bool b) {
    positionable_set_editor_button->set_visible(b);
}

void PositionableSetEditorPlugin::edit(Object* p_object) {
    if (auto* positionable_set {Object::cast_to<resource::PositionableSet>(p_object)}) {
        positionable_set_editor->set_positionable_set(Ref<resource::PositionableSet>(positionable_set));
    }
}

PositionableSetEditorPlugin::PositionableSetEditorPlugin() :
    EditorPlugin(),
    positionable_set_editor(nullptr),
    positionable_set_editor_button(nullptr) {}

void PositionableSetEditorPlugin::_bind_methods() {}

#endif