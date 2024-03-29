#ifdef TOOLS_ENABLED

#include "isometric_tile_editor_plugin.h"

#include "isometric_string_names.h"
#include "logging.h"
#include "outline_drawer.h"

#include <editor/editor_node.h>
#include <node/isometric_map.h>

using namespace editor;

static constexpr const char* OUTLINE_COLOR_TITLE{"Outline color:"};

IsometricTileEditorPlugin* IsometricTileEditorPlugin::get_instance() {
    static IsometricTileEditorPlugin* instance {nullptr};
    if (unlikely(!instance)) {
        instance = memnew(IsometricTileEditorPlugin);
    }
    return instance;
}

bool IsometricTileEditorPlugin::handles(Object* p_object) const {
    return cast_to<node::IsometricPositionable>(p_object) && !cast_to<node::IsometricMap>(p_object);
}

void IsometricTileEditorPlugin::_notification(int p_notification) {
    if (p_notification != NOTIFICATION_READY) {
        return;
    }

    add_control_to_container(CustomControlContainer::CONTAINER_CANVAS_EDITOR_MENU, toolbar);

    toolbar->add_child(memnew(Label(OUTLINE_COLOR_TITLE)));
    toolbar->add_child(color_picker_button);
}

void IsometricTileEditorPlugin::edit(Object* p_object) {
    if (!p_object) {
        return;
    }

    selected_positionable = cast_to<node::IsometricPositionable>(p_object);

    LOG_INFO(selected_positionable);
    LOG_INFO(get_tree()->get_edited_scene_root()->get_node(selected_positionable->get_path().rel_path_to(get_tree()->get_edited_scene_root()->get_path())));

    LOG_INFO(selected_positionable->get_path().rel_path_to(get_tree()->get_edited_scene_root()->get_path()));

    OutlineData& outline_data {selected_positionable->get_outline_data()};
    outline_data.line_size = 1.0;
    _on_color_picker_change(color_picker_button->get_pick_color());
    OutlineDrawer::draw_outline(selected_positionable);
    OutlineDrawer::set_outline_visible(selected_positionable, true);

    if (!selected_positionable->is_connected(IsometricStringNames::get_singleton()->size_changed_signal, Callable(this, "_on_size_changed"))) {
        selected_positionable->connect(IsometricStringNames::get_singleton()->size_changed_signal, Callable(this, "_on_size_changed"));
    }
}

void IsometricTileEditorPlugin::make_visible(bool p_visible) {
    toolbar->set_visible(p_visible);
}

void IsometricTileEditorPlugin::_on_size_changed() {
    OutlineDrawer::draw_outline(selected_positionable);
}

void IsometricTileEditorPlugin::_on_color_picker_change(const Color& p_color) {
    selected_positionable->get_outline_data().color = p_color;
    OutlineDrawer::draw_outline(selected_positionable);
}

IsometricTileEditorPlugin::IsometricTileEditorPlugin() :
  selected_positionable{nullptr},
  toolbar{ memnew(HBoxContainer) },
  color_picker_button{ memnew(ColorPickerButton) } {

    toolbar->hide();
    color_picker_button->set_text(OUTLINE_COLOR_TITLE);
    color_picker_button->connect("color_changed", Callable(this, "_on_color_picker_change"));
}

void IsometricTileEditorPlugin::_bind_methods() {
    ClassDB::bind_method("_on_size_changed", &IsometricTileEditorPlugin::_on_size_changed);
    ClassDB::bind_method(D_METHOD("_on_color_picker_change", "p_color"), &IsometricTileEditorPlugin::_on_color_picker_change);
}

#endif