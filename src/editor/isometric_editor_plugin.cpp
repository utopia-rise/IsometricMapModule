#ifdef TOOLS_ENABLED

#include "isometric_editor_plugin.h"

#include "constants.h"
#include "isometric_server.h"
#include "isometric_string_names.h"
#include "outline_drawer.h"
#include "positionable_scenes_cache_manager.h"
#include "positionable_selector_manager.h"

#include <core/os/keyboard.h>
#include <scene/main/viewport.h>

using namespace editor;

static constexpr const char* POSITIONABLE_PANE_BUTTON_TITLE {"Isometric positionables"};

static constexpr const char* NONE_EDITION_LABEL {"None"};
static constexpr const char* SELECT_EDITION_LABEL {"Select"};
static constexpr const char* PAINT_EDITION_LABEL {"Paint"};
static constexpr const char* DRAG_AND_DROP_EDITION_LABEL {"Drag & Drop"};
static constexpr const char* GRID_COLOR_PICKER_TITLE {"Grid color:"};

editor::inspector::PositionableSelectionPane* IsometricEditorPlugin::get_selection_pane() const {
    return positionable_selection_pane;
}

IsometricEditorPlugin::IsometricEditorPlugin() :
  toolbar {nullptr},
  positionable_selection_pane {nullptr},
  positionable_pane_button {nullptr},
  grid_color_picker_button {memnew(ColorPickerButton)},
  edition_mode_button(memnew(OptionButton)),
  debug_button {nullptr},
  selected_map {nullptr},
  show_debug(false),
  current_mode(Mode::NONE),
  should_clear_buffer_on_next_frame(),
  painting_command_emitter(),
  select_command_emitter(),
  select_all_command_emitter(),
  delete_command_emitter(),
  drag_and_drop_command_emitter(),
  move_editor_drawer_command_emitter(),
  rotate_editor_plane_command_emitter(),
  plane_view_limiter_command_emitter() {
    grid_color_picker_button->set_text(GRID_COLOR_PICKER_TITLE);
    grid_color_picker_button->connect("color_changed", Callable(this, "_on_grid_color_picker_change"));
}

IsometricEditorPlugin::~IsometricEditorPlugin() {
    PositionableScenesCacheManager::get_instance().clear();
}

IsometricEditorPlugin* IsometricEditorPlugin::get_instance() {
    static IsometricEditorPlugin* instance {nullptr};
    if (unlikely(!instance)) {
        instance = memnew(IsometricEditorPlugin);
    }
    return instance;
}

void IsometricEditorPlugin::set_debug_mode(bool b) {
    show_debug = b;
    get_tree()->call_group(IsometricStringNames::get_singleton()->debug_group_name, "set_debug_view", b);
    ISOMETRIC_SERVER->set_debug(b);
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

        toolbar->add_child(memnew(Label(GRID_COLOR_PICKER_TITLE)));
        toolbar->add_child(grid_color_picker_button);

        edition_mode_button->add_item(NONE_EDITION_LABEL);
        edition_mode_button->add_item(SELECT_EDITION_LABEL);
        edition_mode_button->add_item(PAINT_EDITION_LABEL);
        edition_mode_button->add_item(DRAG_AND_DROP_EDITION_LABEL);
        edition_mode_button->set_flat(true);
        edition_mode_button->connect("item_selected", Callable(this, "_on_edition_mode_changed"));
        toolbar->add_child(edition_mode_button);

        debug_button = memnew(Button);
        debug_button->set_flat(true);
        debug_button->connect("toggled", Callable(this, "set_debug_mode"));
        debug_button->set_toggle_mode(true);
        debug_button->set_text("Debug");
        toolbar->add_child(debug_button);

        // Add to editor bottom
        positionable_selection_pane = memnew(editor::inspector::PositionableSelectionPane);
        positionable_pane_button = add_control_to_bottom_panel(positionable_selection_pane, POSITIONABLE_PANE_BUTTON_TITLE);
        positionable_pane_button->set_visible(false);

        RenderingServer::get_singleton()->connect("frame_post_draw", Callable(this, "_on_frame_post_draw"));
    }
}

void IsometricEditorPlugin::edit(Object* p_object) {
    selected_map = cast_to<node::IsometricMap>(p_object);

    if (!selected_map->is_connected("draw", Callable(this, "refresh"))) {
        selected_map->connect("draw", Callable(this, "refresh").bind(EditorPlane::PlaneType::EDITOR_DRAWER));
    }
    positionable_selection_pane->set_positionable_set(selected_map->get_positionable_set());
    if (!selected_map->is_connected("positional_set_changed", Callable(positionable_selection_pane, "set_positionable_set"))) {
        selected_map->connect("positional_set_changed", Callable(positionable_selection_pane, "set_positionable_set"));
    }

    if (!selected_map->is_connected(IsometricStringNames::get_singleton()->size_changed_signal, Callable(this, "_on_map_size_changed"))) {
        selected_map->connect(IsometricStringNames::get_singleton()->size_changed_signal, Callable(this, "_on_map_size_changed"));
    }

    auto index {reinterpret_cast<uint64_t>(selected_map)};
    if (!handling_data_map.has(index)) { handling_data_map[index] = MapHandlingData(selected_map->get_size()); }
    editor::OutlineDrawer::set_outline_visible(selected_map, show_debug);

    _draw_edition_grid();

    editor::PositionableSelectorManager::get_instance().refresh_outline_for_selected(selected_map);
}

void IsometricEditorPlugin::drop() {
    if (selected_map) {
        editor::OutlineDrawer::set_outline_visible(selected_map, false);
        if (selected_map->is_connected("draw", Callable(this, "refresh"))) {
            selected_map->disconnect("draw", Callable(this, "refresh"));
        }
        auto index {reinterpret_cast<uint64_t>(selected_map)};

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
    if (!selected_map) { return false; }
    int id = EditorUndoRedoManager::get_singleton()->get_history_id_for_object(selected_map);
    UndoRedo* undo_redo = EditorUndoRedoManager::get_singleton()->get_history_undo_redo(id);

    Ref<InputEventKey> keyboard_event {p_event};
    if (keyboard_event.is_valid() && keyboard_event->is_pressed()) {
        Key key = keyboard_event->get_keycode_with_modifiers();
        if (keyboard_event->is_command_or_control_pressed()) {
            switch (key) {
                case Key::Z:
                    if (keyboard_event->is_shift_pressed()) {
                        undo_redo->redo();
                    } else {
                        undo_redo->undo();
                    }
                    return true;
                case Key::Y:
                    EditorUndoRedoManager::get_singleton()->redo();
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
            select_command_emitter.on_gui_input(p_event, undo_redo);
            select_all_command_emitter.on_gui_input(p_event, undo_redo);
            delete_command_emitter.on_gui_input(p_event, undo_redo);
            break;
        case PAINT:
            painting_command_emitter.on_gui_input(p_event, undo_redo);
            break;
        case DRAG_AND_DROP:
            drag_and_drop_command_emitter.on_gui_input(p_event, undo_redo);
            break;
    }
    move_editor_drawer_command_emitter.on_gui_input(p_event, undo_redo);
    rotate_editor_plane_command_emitter.on_gui_input(p_event, undo_redo);

    plane_view_limiter_command_emitter.on_gui_input(p_event, undo_redo);
    return true;
}

void IsometricEditorPlugin::make_visible(bool b) {
    if (!b) { drop(); }
    toolbar->set_visible(b);
    positionable_pane_button->set_visible(b);
}

void IsometricEditorPlugin::refresh(int p_plane_type) {
    auto index {reinterpret_cast<uint64_t>(selected_map)};
    if (!handling_data_map.has(index)) { return; }

    MapHandlingData& map_handling_data = handling_data_map[index];
    if (map_handling_data.is_grid[p_plane_type]) {
        _draw_edition_grid();
    } else {
        auto plane_type {static_cast<EditorPlane::PlaneType>(p_plane_type)};
        _draw_plane(plane_type);
        _set_plane_timer(plane_type, 1.5);
    }
}

node::IsometricMap* IsometricEditorPlugin::get_selected_map() const {
    return selected_map;
}

EditorPlane& IsometricEditorPlugin::get_editor_plane_for_selected_map(EditorPlane::PlaneType p_plane_type) {
    return handling_data_map[reinterpret_cast<uint64_t>(selected_map)].editor_planes[p_plane_type];
}

bool IsometricEditorPlugin::is_aabb_in_view_limiters(const AABB& p_aabb) const {
    const MapHandlingData& map_handling_data {handling_data_map[reinterpret_cast<uint64_t>(selected_map)]};
    const Vector3& position {p_aabb.position};
    const Vector3& size {p_aabb.size};
    int pos_x {static_cast<int>(position.x)};
    int pos_y {static_cast<int>(position.y)};
    int pos_z {static_cast<int>(position.z)};
    int max_x {pos_x + static_cast<int>(size.x) - 1};
    int max_y {pos_y + static_cast<int>(size.y) - 1};
    int max_z {pos_z + static_cast<int>(size.z) - 1};

    if (pos_x < map_handling_data.editor_planes[EditorPlane::PlaneType::X_MIN_VIEW_LIMITER].get_position() ||
        pos_y < map_handling_data.editor_planes[EditorPlane::PlaneType::Y_MIN_VIEW_LIMITER].get_position() ||
        pos_z < map_handling_data.editor_planes[EditorPlane::PlaneType::Z_MIN_VIEW_LIMITER].get_position() ||
        max_x >= map_handling_data.editor_planes[EditorPlane::PlaneType::X_MAX_VIEW_LIMITER].get_position() ||
        max_y >= map_handling_data.editor_planes[EditorPlane::PlaneType::Y_MAX_VIEW_LIMITER].get_position() ||
        max_z >= map_handling_data.editor_planes[EditorPlane::PlaneType::Z_MAX_VIEW_LIMITER].get_position()) {
        return false;
    }
    return true;
}

IsometricEditorPlugin::MapHandlingData::MapHandlingData() :
  editor_planes {
    {0, Vector3::Axis::AXIS_Z, Vector2()},
    {-1, Vector3::Axis::AXIS_X, Vector2()},
    {-1, Vector3::Axis::AXIS_X, Vector2()},
    {-1, Vector3::Axis::AXIS_Y, Vector2()},
    {-1, Vector3::Axis::AXIS_Y, Vector2()},
    {-1, Vector3::Axis::AXIS_Z, Vector2()},
    {-1, Vector3::Axis::AXIS_Z, Vector2()}},
  plane_timers {
    Ref<SceneTreeTimer>(),
    Ref<SceneTreeTimer>(),
    Ref<SceneTreeTimer>(),
    Ref<SceneTreeTimer>(),
    Ref<SceneTreeTimer>(),
    Ref<SceneTreeTimer>(),
    Ref<SceneTreeTimer>()},
  is_grid {true, false, false, false, false, false, false} {}

IsometricEditorPlugin::MapHandlingData::MapHandlingData(const Vector3& p_map_size) :
  editor_planes {
    {0, Vector3::Axis::AXIS_Z, {p_map_size.x, p_map_size.y}},
    {0, Vector3::Axis::AXIS_X, {p_map_size.y, p_map_size.z}},
    {Constants::int_max, Vector3::Axis::AXIS_X, {p_map_size.y, p_map_size.z}},
    {0, Vector3::Axis::AXIS_Y, {p_map_size.x, p_map_size.z}},
    {Constants::int_max, Vector3::Axis::AXIS_Y, {p_map_size.x, p_map_size.z}},
    {0, Vector3::Axis::AXIS_Z, {p_map_size.x, p_map_size.y}},
    {Constants::int_max, Vector3::Axis::AXIS_Z, {p_map_size.x, p_map_size.y}}},
  plane_timers {{nullptr}, {nullptr}, {nullptr}, {nullptr}, {nullptr}, {nullptr}, {nullptr}},
  is_grid {true, false, false, false, false, false, false} {}

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
    const String& selected_label {edition_mode_button->get_item_text(selected_index)};
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

void IsometricEditorPlugin::_on_plane_visibility_timeout(int p_plane_type) {
    auto index {reinterpret_cast<uint64_t>(selected_map)};
    MapHandlingData& map_handling_data {handling_data_map[index]};
    EditionGridDrawer::clear_for_editor_plane(map_handling_data.editor_planes[p_plane_type]);
    Ref<SceneTreeTimer> timer = map_handling_data.plane_timers[p_plane_type];
    if (!timer.is_valid()) { return; }
    map_handling_data.plane_timers[p_plane_type] = Ref<SceneTreeTimer>();
}

void IsometricEditorPlugin::_on_map_size_changed() {
    refresh(EditorPlane::PlaneType::EDITOR_DRAWER);
}

void IsometricEditorPlugin::_on_grid_color_picker_change(const Color& p_color) {
    _draw_edition_grid();
}

void IsometricEditorPlugin::_draw_edition_grid() const {
    auto index {reinterpret_cast<uint64_t>(selected_map)};
    const MapHandlingData& map_handling_data {handling_data_map[index]};
    EditionGridDrawer::draw_grid(
      map_handling_data.editor_planes[EditorPlane::PlaneType::EDITOR_DRAWER],
      selected_map,
      grid_color_picker_button->get_pick_color()
    );
}

void IsometricEditorPlugin::_draw_plane(EditorPlane::PlaneType p_plane_type) {
    auto index {reinterpret_cast<uint64_t>(selected_map)};
    const EditorPlane& plane {handling_data_map[index].editor_planes[p_plane_type]};
    EditionGridDrawer::draw_plane(plane, selected_map);
}

void IsometricEditorPlugin::_set_plane_timer(EditorPlane::PlaneType p_plane_type, float p_delay) {
    static const StringName timeoutSignalName {"timeout"};

    auto index {reinterpret_cast<uint64_t>(selected_map)};
    MapHandlingData& map_handling_data {handling_data_map[index]};

    Ref<SceneTreeTimer> existing_timer = map_handling_data.plane_timers[p_plane_type];

    if (existing_timer.is_valid()) {
        existing_timer->release_connections();
        map_handling_data.plane_timers[p_plane_type] = Ref<SceneTreeTimer>();
    }

    Ref<SceneTreeTimer> timer {SceneTree::get_singleton()->create_timer(p_delay, false)};
    timer->connect(timeoutSignalName, Callable(this, "_on_plane_visibility_timeout").bind(p_plane_type));
    map_handling_data.plane_timers[p_plane_type] = timer;
}

void IsometricEditorPlugin::_bind_methods() {
    ClassDB::bind_method("set_debug_mode", &IsometricEditorPlugin::set_debug_mode);
    ClassDB::bind_method(D_METHOD("refresh", "p_plane_type"), &IsometricEditorPlugin::refresh);
    ClassDB::bind_method("_on_frame_post_draw", &IsometricEditorPlugin::_on_frame_post_draw);
    ClassDB::bind_method("_on_edition_mode_changed", &IsometricEditorPlugin::_on_edition_mode_changed);
    ClassDB::bind_method(D_METHOD("_on_plane_visibility_timeout", "p_plane_type"), &IsometricEditorPlugin::_on_plane_visibility_timeout);
    ClassDB::bind_method("_on_map_size_changed", &IsometricEditorPlugin::_on_map_size_changed);
    ClassDB::bind_method(D_METHOD("_on_grid_color_picker_change", "p_color"), &IsometricEditorPlugin::_on_grid_color_picker_change);
}

#endif
