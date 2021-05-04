#ifdef TOOLS_ENABLED

#include "isometric_editor_plugin.h"

using namespace editor;

IsometricEditorPlugin::IsometricEditorPlugin() :
        undo_redo{EditorNode::get_undo_redo()},
        toolbar{nullptr},
        debug_button{nullptr},
        selected_map{nullptr},
        show_debug(false) {
}

IsometricEditorPlugin* IsometricEditorPlugin::get_instance() {
    static IsometricEditorPlugin* instance{memnew(IsometricEditorPlugin)};
    return instance;
}

void IsometricEditorPlugin::set_debug_mode(bool b) {
    show_debug = b;
    selected_map->set_debug(b);
}

void IsometricEditorPlugin::_notification(int p_notification) {
    if(p_notification == NOTIFICATION_READY){
        // Add menu items.
        toolbar = memnew(HBoxContainer);
        toolbar->hide();
        add_control_to_container(CustomControlContainer::CONTAINER_CANVAS_EDITOR_MENU ,toolbar);

        debug_button = memnew(Button);
        debug_button->set_flat(true);
        debug_button->connect("toggled", this, "set_debug_mode");
        debug_button->set_toggle_mode(true);
        debug_button->set_text("Debug");
        toolbar->add_child(debug_button);
    }
}

void IsometricEditorPlugin::edit(Object* p_object) {
    selected_map = cast_to<node::IsometricMap>(p_object);
    auto index{reinterpret_cast<uint64_t>(selected_map)};
    if (!handling_data_map.has(index)) {
        const Vector3& map_size{selected_map->get_size()};
        handling_data_map[index] = MapHandlingData({0, EditorAxes::Z, {map_size.x, map_size.y}});
    }
    selected_map->set_debug(show_debug);
}

bool IsometricEditorPlugin::handles(Object* p_object) const {
    return cast_to<node::IsometricMap>(p_object);
}

void IsometricEditorPlugin::clear() {
    selected_map->set_debug(false);
    selected_map = nullptr;
}

bool IsometricEditorPlugin::forward_canvas_gui_input(const Ref<InputEvent>& p_event) {
    return false;
}

void IsometricEditorPlugin::_bind_methods() {
    ClassDB::bind_method("set_debug_mode", &IsometricEditorPlugin::set_debug_mode);
}

void IsometricEditorPlugin::make_visible(bool b) {
    if(!b){
        selected_map->set_debug(false);
    }
    toolbar->set_visible(b);
}


IsometricEditorPlugin::MapHandlingData::MapHandlingData() : edition_grid_plane{0, EditorAxes::NONE, Vector2()} {

}

IsometricEditorPlugin::MapHandlingData::MapHandlingData(EditorPlane p_editor_plane) : edition_grid_plane(p_editor_plane) {

}

#endif
