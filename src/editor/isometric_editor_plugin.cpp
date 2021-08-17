#ifdef TOOLS_ENABLED

#include "isometric_editor_plugin.h"
#include "positionable_scenes_cache_manager.h"
#include <scene/main/viewport.h>
#include <core/os/keyboard.h>

using namespace editor;

const char* POSITIONABLE_PANE_BUTTON_TITLE{"Isometric positionables"};

const char* NONE_EDITION_LABEL{"None"};
const char* SELECT_EDITION_LABEL{"Select"};
const char* PAINT_EDITION_LABEL{"Paint"};

editor::inspector::PositionableSelectionPane* IsometricEditorPlugin::get_selection_pane() const {
    return positionable_selection_pane;
}

IsometricEditorPlugin::IsometricEditorPlugin() :
        undo_redo{EditorNode::get_undo_redo()},
        toolbar{nullptr},
        positionable_selection_pane{nullptr},
        positionable_pane_button{nullptr},
        edition_mode_button(memnew(OptionButton)),
        debug_button{nullptr},
        selected_map{nullptr},
        show_debug(false),
        current_mode(Mode::NONE),
        edition_grid_drawer(),
        should_clear_buffer_on_next_frame(),
        painting_command_emitter(EditorNode::get_undo_redo()),
        select_command_emitter(EditorNode::get_undo_redo()),
        delete_command_emitter(EditorNode::get_undo_redo()) {
}

IsometricEditorPlugin::~IsometricEditorPlugin() {
    PositionableScenesCacheManager::get_instance().clear();
}

IsometricEditorPlugin* IsometricEditorPlugin::get_instance() {
    static IsometricEditorPlugin* instance{memnew(IsometricEditorPlugin)};
    return instance;
}

void IsometricEditorPlugin::set_debug_mode(bool b) {
    show_debug = b;
    selected_map->show_outline(b);
}

void IsometricEditorPlugin::set_should_clear_buffer_on_next_frame(bool should) {
    should_clear_buffer_on_next_frame = should;
}

void IsometricEditorPlugin::refresh_positionable_selection_pane() {
    positionable_selection_pane->refresh_path_selector();
}

void IsometricEditorPlugin::_notification(int p_notification) {
    if (p_notification == NOTIFICATION_READY) {
        // Add menu items.
        toolbar = memnew(HBoxContainer);
        toolbar->hide();
        add_control_to_container(CustomControlContainer::CONTAINER_CANVAS_EDITOR_MENU, toolbar);

        edition_mode_button->add_item(NONE_EDITION_LABEL);
        edition_mode_button->add_item(SELECT_EDITION_LABEL);
        edition_mode_button->add_item(PAINT_EDITION_LABEL);
        edition_mode_button->set_flat(true);
        edition_mode_button->connect("item_selected", this, "_on_edition_mode_changed");
        toolbar->add_child(edition_mode_button);

        debug_button = memnew(Button);
        debug_button->set_flat(true);
        debug_button->connect("toggled", this, "set_debug_mode");
        debug_button->set_toggle_mode(true);
        debug_button->set_text("Debug");
        toolbar->add_child(debug_button);

        //Add to editor bottom
        positionable_selection_pane = memnew(editor::inspector::PositionableSelectionPane);
        positionable_pane_button = add_control_to_bottom_panel(positionable_selection_pane, POSITIONABLE_PANE_BUTTON_TITLE);
        positionable_pane_button->set_visible(false);

        VisualServer::get_singleton()->connect("frame_post_draw", this, "_on_frame_post_draw");
    }
}


void IsometricEditorPlugin::edit(Object* p_object) {
    selected_map = cast_to<node::IsometricMap>(p_object);

    painting_command_emitter.set_map(selected_map);
    select_command_emitter.set_map(selected_map);
    delete_command_emitter.set_map(selected_map);

    if (!selected_map->is_connected("draw", this, "refresh")) {
        selected_map->connect("draw", this, "refresh");
    }
    positionable_selection_pane->set_positionable_set(selected_map->get_positionable_set());
    if (!selected_map->is_connected("positional_set_changed", positionable_selection_pane, "set_positionable_set")) {
        selected_map->connect("positional_set_changed", positionable_selection_pane, "set_positionable_set");
    }


    auto index{reinterpret_cast<uint64_t>(selected_map)};
    if (!handling_data_map.has(index)) {
        const Vector3& map_size{selected_map->get_size()};
        handling_data_map[index] = MapHandlingData({0, EditorAxes::Z, {map_size.x, map_size.y}});
    }
    selected_map->show_outline(show_debug);
    edition_grid_drawer.draw_grid(handling_data_map[index].edition_grid_plane, *selected_map);
}

void IsometricEditorPlugin::drop() {
    if (selected_map) {
        selected_map->show_outline(false);
        if (selected_map->is_connected("draw", this, "refresh")) {
            selected_map->disconnect("draw", this, "refresh");
        }
        auto index{reinterpret_cast<uint64_t>(selected_map)};
        edition_grid_drawer.clear_grid(handling_data_map[index].edition_grid_plane);
    }
}

bool IsometricEditorPlugin::handles(Object* p_object) const {
    return cast_to<node::IsometricMap>(p_object);
}

void IsometricEditorPlugin::clear() {
    if (selected_map) {
        selected_map->show_outline(false);
        selected_map = nullptr;
    }
}

bool IsometricEditorPlugin::forward_canvas_gui_input(const Ref<InputEvent>& p_event) {
    if (!selected_map) {
        return false;
    }

    Ref<InputEventKey> keyboard_event{p_event};
    if (keyboard_event.is_valid() && keyboard_event->is_pressed()) {
        bool is_ctrl{
#ifdef APPLE_STYLE_KEYS
            keyboard_event->get_command()
#else
            keyboard_event->get_control()
#endif
        };
        if (is_ctrl) {
            uint32_t key_pressed{keyboard_event->get_scancode()};
            switch (key_pressed) {
                case KEY_Z:
                    if (keyboard_event->get_shift()) {
                        EditorNode::get_undo_redo()->redo();
                    } else {
                        EditorNode::get_undo_redo()->undo();
                    }
                    return true;
                case KEY_Y:
                    EditorNode::get_undo_redo()->redo();
                    return true;
                default:
                    break;
            }
        }
    }

    switch (current_mode) {
        case NONE:
            return false;
        case SELECT:
            select_command_emitter.on_gui_input(p_event);
            delete_command_emitter.on_gui_input(p_event);
            break;
        case PAINT:
            painting_command_emitter.on_gui_input(p_event);
            break;
    }
    return true;
}

void IsometricEditorPlugin::make_visible(bool b) {
    if (!b) {
        drop();
    }
    toolbar->set_visible(b);
    positionable_pane_button->set_visible(b);
}

void IsometricEditorPlugin::refresh() const {
    auto index{reinterpret_cast<uint64_t>(selected_map)};
    if (!handling_data_map.has(index)) {
        return;
    }
    edition_grid_drawer.draw_grid(handling_data_map[index].edition_grid_plane, *selected_map);
}

IsometricEditorPlugin::MapHandlingData::MapHandlingData() : edition_grid_plane{0, EditorAxes::NONE, Vector2()} {

}

IsometricEditorPlugin::MapHandlingData::MapHandlingData(EditorPlane p_editor_plane) : edition_grid_plane(
        p_editor_plane) {

}

void IsometricEditorPlugin::_on_frame_post_draw() {
    if (PositionableScenesCacheManager::get_instance().is_adding()) {
        should_clear_buffer_on_next_frame = false;
    } else {
        if (should_clear_buffer_on_next_frame) {
            PositionableScenesCacheManager::get_instance().copy_current_viewports_textures();
            PositionableScenesCacheManager::get_instance().refresh_all_icons();
            PositionableScenesCacheManager::get_instance().clear_current_viewports();
            should_clear_buffer_on_next_frame = false;
        } else {
            should_clear_buffer_on_next_frame = true;
        }
    }
}

void IsometricEditorPlugin::_on_edition_mode_changed(int selected_index) {
    const String& selected_label{edition_mode_button->get_item_text(selected_index)};
    if (selected_label == NONE_EDITION_LABEL) {
        current_mode = Mode::NONE;
    } else if (selected_label == SELECT_EDITION_LABEL) {
        current_mode = Mode::SELECT;
    } else if (selected_label == PAINT_EDITION_LABEL) {
        current_mode = Mode::PAINT;
    }
}

void IsometricEditorPlugin::_bind_methods() {
    ClassDB::bind_method("set_debug_mode", &IsometricEditorPlugin::set_debug_mode);
    ClassDB::bind_method("refresh", &IsometricEditorPlugin::refresh);
    ClassDB::bind_method("_on_frame_post_draw", &IsometricEditorPlugin::_on_frame_post_draw);
    ClassDB::bind_method("_on_edition_mode_changed", &IsometricEditorPlugin::_on_edition_mode_changed);
}

#endif
