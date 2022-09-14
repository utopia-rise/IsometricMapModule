#ifdef TOOLS_ENABLED

#include "isometric_editor_plugin.h"
#include "positionable_scenes_cache_manager.h"
#include "positionable_selector_manager.h"
#include "outline_drawer.h"
#include <scene/main/viewport.h>
#include <core/os/keyboard.h>

using namespace editor;

static constexpr const char* POSITIONABLE_PANE_BUTTON_TITLE{"Isometric positionables"};

static constexpr const char* NONE_EDITION_LABEL{"None"};
static constexpr const char* SELECT_EDITION_LABEL{"Select"};
static constexpr const char* PAINT_EDITION_LABEL{"Paint"};
static constexpr const char* DRAG_AND_DROP_EDITION_LABEL{"Drag & Drop"};

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
        should_clear_buffer_on_next_frame(),
        painting_command_emitter(EditorNode::get_undo_redo()),
        select_command_emitter(EditorNode::get_undo_redo()),
        select_all_command_emitter(EditorNode::get_undo_redo()),
        delete_command_emitter(EditorNode::get_undo_redo()),
        drag_and_drop_command_emitter(EditorNode::get_undo_redo()),
        move_editor_drawer_command_emitter(EditorNode::get_undo_redo()),
        rotate_editor_plane_command_emitter(EditorNode::get_undo_redo()),
        plane_view_limiter_command_emitter(EditorNode::get_undo_redo()) {
}

IsometricEditorPlugin::~IsometricEditorPlugin() {
    PositionableScenesCacheManager::get_instance().clear();
}

IsometricEditorPlugin* IsometricEditorPlugin::get_instance() {
    static IsometricEditorPlugin* instance{nullptr};
    if (unlikely(!instance && ObjectDB::instance_validate(EditorNode::get_undo_redo()))) {
        instance = memnew(IsometricEditorPlugin);
    }
    return instance;
}

void IsometricEditorPlugin::set_debug_mode(bool b) {
    show_debug = b;
    editor::OutlineDrawer::set_outline_visible(selected_map, b);
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
        edition_mode_button->add_item(DRAG_AND_DROP_EDITION_LABEL);
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

    if (!selected_map->is_connected("draw", this, "refresh")) {
        selected_map->connect("draw", this, "refresh");
    }
    positionable_selection_pane->set_positionable_set(selected_map->get_positionable_set());
    if (!selected_map->is_connected("positional_set_changed", positionable_selection_pane, "set_positionable_set")) {
        selected_map->connect("positional_set_changed", positionable_selection_pane, "set_positionable_set");
    }

    auto index{reinterpret_cast<uint64_t>(selected_map)};
    if (!handling_data_map.has(index)) {
        handling_data_map[index] = MapHandlingData(selected_map->get_size());
    }
    editor::OutlineDrawer::set_outline_visible(selected_map, show_debug);
    
    _draw_grids_and_planes();

    editor::PositionableSelectorManager::get_instance().refresh_outline_for_selected(selected_map);
}

void IsometricEditorPlugin::drop() {
    if (selected_map && ObjectDB::instance_validate(selected_map)) {
        editor::OutlineDrawer::set_outline_visible(selected_map, false);
        if (selected_map->is_connected("draw", this, "refresh")) {
            selected_map->disconnect("draw", this, "refresh");
        }
        auto index{reinterpret_cast<uint64_t>(selected_map)};

        const MapHandlingData& map_handling_data = handling_data_map[index];

        for (int i = EditorPlane::PlaneType::EDITOR_DRAWER; i < EditorPlane::PlaneType::SIZE; ++i) {
            EditionGridDrawer::clear_for_editor_plane(map_handling_data.editor_planes[i]);
        }
    }
    selected_map = nullptr;
}

bool IsometricEditorPlugin::handles(Object* p_object) const {
    return cast_to<node::IsometricMap>(p_object);
}

void IsometricEditorPlugin::clear() {
    if (selected_map) {
        editor::OutlineDrawer::set_outline_visible(selected_map, false);
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
            select_all_command_emitter.on_gui_input(p_event);
            delete_command_emitter.on_gui_input(p_event);
            break;
        case PAINT:
            painting_command_emitter.on_gui_input(p_event);
            break;
        case DRAG_AND_DROP:
            drag_and_drop_command_emitter.on_gui_input(p_event);
            break;
    }
    move_editor_drawer_command_emitter.on_gui_input(p_event);
    rotate_editor_plane_command_emitter.on_gui_input(p_event);

    plane_view_limiter_command_emitter.on_gui_input(p_event);
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
    
    _draw_grids_and_planes();
}

node::IsometricMap* IsometricEditorPlugin::get_selected_map() const {
    return selected_map;
}

EditorPlane& IsometricEditorPlugin::get_editor_plane_for_selected_map(EditorPlane::PlaneType p_plane_type) {
    return handling_data_map[reinterpret_cast<uint64_t>(selected_map)].editor_planes[p_plane_type];
}

IsometricEditorPlugin::MapHandlingData::MapHandlingData() :
        editor_planes{
                {0,  Vector3::Axis::AXIS_Z, Vector2()},
                {-1, Vector3::Axis::AXIS_X, Vector2()},
                {-1, Vector3::Axis::AXIS_X, Vector2()},
                {-1, Vector3::Axis::AXIS_Y, Vector2()},
                {-1, Vector3::Axis::AXIS_Y, Vector2()},
                {-1, Vector3::Axis::AXIS_Z, Vector2()},
                {-1, Vector3::Axis::AXIS_Z, Vector2()}
        } {

}

IsometricEditorPlugin::MapHandlingData::MapHandlingData(const Vector3& p_map_size) :
        editor_planes{
                {0, Vector3::Axis::AXIS_Z, {p_map_size.x, p_map_size.y}},
                {0, Vector3::Axis::AXIS_X, {p_map_size.y, p_map_size.z}},
                {static_cast<int>(p_map_size.x), Vector3::Axis::AXIS_X, {p_map_size.y, p_map_size.z}},
                {0, Vector3::Axis::AXIS_Y, {p_map_size.x, p_map_size.z}},
                {static_cast<int>(p_map_size.y), Vector3::Axis::AXIS_Y, {p_map_size.x, p_map_size.z}},
                {0, Vector3::Axis::AXIS_Z, {p_map_size.x, p_map_size.y}},
                {static_cast<int>(p_map_size.z), Vector3::Axis::AXIS_Z, {p_map_size.x, p_map_size.y}}
        } {

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
    } else if (selected_label == DRAG_AND_DROP_EDITION_LABEL) {
        current_mode = Mode::DRAG_AND_DROP;
    }
}

void IsometricEditorPlugin::_draw_grids_and_planes() const {
    auto index{reinterpret_cast<uint64_t>(selected_map)};
    const MapHandlingData& map_handling_data{handling_data_map[index]};
    EditionGridDrawer::draw_grid(map_handling_data.editor_planes[EditorPlane::PlaneType::EDITOR_DRAWER], *selected_map);
    const Vector3& map_size{selected_map->get_size()};

    for (int i = EditorPlane::PlaneType::X_MIN_VIEW_LIMITER; i <= EditorPlane::PlaneType::Z_MAX_VIEW_LIMITER; ++i) {
        const EditorPlane& plane{map_handling_data.editor_planes[i]};
        switch (plane.get_axis()) {
            case Vector3::AXIS_X:
                if ((plane.get_position() >= map_size.x && i == EditorPlane::PlaneType::X_MAX_VIEW_LIMITER) || (plane.get_position() <= 0 && i == EditorPlane::PlaneType::X_MIN_VIEW_LIMITER)) {
                    EditionGridDrawer::clear_for_editor_plane(plane);
                    continue;
                }
                break;
            case Vector3::AXIS_Y:
                if ((plane.get_position() >= map_size.y && i == EditorPlane::PlaneType::Y_MAX_VIEW_LIMITER) || (plane.get_position() <= 0 && i == EditorPlane::PlaneType::Y_MIN_VIEW_LIMITER)) {
                    EditionGridDrawer::clear_for_editor_plane(plane);
                    continue;
                }
                break;
            case Vector3::AXIS_Z:
                if ((plane.get_position() >= map_size.z && i == EditorPlane::PlaneType::Z_MAX_VIEW_LIMITER) || (plane.get_position() <= 0 && i == EditorPlane::PlaneType::Z_MIN_VIEW_LIMITER)) {
                    EditionGridDrawer::clear_for_editor_plane(plane);
                    continue;
                }
                break;
        }
        EditionGridDrawer::draw_plane(plane, *selected_map);
    }
}

void IsometricEditorPlugin::_bind_methods() {
    ClassDB::bind_method("set_debug_mode", &IsometricEditorPlugin::set_debug_mode);
    ClassDB::bind_method("refresh", &IsometricEditorPlugin::refresh);
    ClassDB::bind_method("_on_frame_post_draw", &IsometricEditorPlugin::_on_frame_post_draw);
    ClassDB::bind_method("_on_edition_mode_changed", &IsometricEditorPlugin::_on_edition_mode_changed);
}

#endif
