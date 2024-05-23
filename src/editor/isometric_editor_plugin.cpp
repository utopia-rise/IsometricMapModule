#ifdef TOOLS_ENABLED

#include "isometric_editor_plugin.h"

#include "constants.h"
#include "editor/commands/move_to_layer_command.h"
#include "editor/commands/revert_command.h"
#include "isometric_server.h"
#include "isometric_string_names.h"
#include "outline_drawer.h"
#include "positionable_scenes_cache_manager.h"
#include "positionable_selector_manager.h"

using namespace editor;

static constexpr const char* POSITIONABLE_PANE_BUTTON_TITLE {"Isometric positionables"};

static constexpr const char* NONE_EDITION_LABEL {"None"};
static constexpr const char* SELECT_EDITION_LABEL {"Select"};
static constexpr const char* PAINT_EDITION_LABEL {"Paint"};
static constexpr const char* DRAG_AND_DROP_EDITION_LABEL {"Drag & Drop"};
static constexpr const char* GRID_COLOR_PICKER_TITLE {"Grid color:"};

static constexpr const char* DEBUG_BUTTON_TITLE {"Debug"};
static constexpr const char* MOVE_SELECTION_TO_CURRENT_LAYER_BUTTON_TITLE {"->"};
static constexpr const char* MOVE_SELECTION_TO_CURRENT_LAYER_BUTTON_TOOLTIP {
  "Move selection to current layer"
};

static constexpr const char MOVE_TO_CURRENT_LAYER_COMMAND[] {"move_to_layer"};

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
  move_selection_to_current_layer_button {nullptr},
  selected_map {nullptr},
  show_debug(false),
  current_mode(Mode::NONE),
  should_clear_buffer_on_next_frame(),
  select_command_emitter(),
  select_all_command_emitter(),
  delete_command_emitter(),
  move_selection_command_emitter(),
  move_editor_drawer_command_emitter(),
  rotate_editor_plane_command_emitter(),
  layers_editor()
  {
    grid_color_picker_button->set_text(GRID_COLOR_PICKER_TITLE);
    grid_color_picker_button->connect("color_changed", Callable(this, "_on_grid_color_picker_change"));
}

IsometricEditorPlugin::~IsometricEditorPlugin() {
    PositionableScenesCacheManager::get_instance().shutdown();
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
        debug_button->set_text(DEBUG_BUTTON_TITLE);
        toolbar->add_child(debug_button);

        move_selection_to_current_layer_button = memnew(Button);
        move_selection_to_current_layer_button->set_flat(true);
        move_selection_to_current_layer_button->set_text(MOVE_SELECTION_TO_CURRENT_LAYER_BUTTON_TITLE);
        move_selection_to_current_layer_button->set_tooltip_text(MOVE_SELECTION_TO_CURRENT_LAYER_BUTTON_TOOLTIP);
        move_selection_to_current_layer_button->connect("pressed", callable_mp(this, &IsometricEditorPlugin::_on_move_selection_to_current_layer));
        toolbar->add_child(move_selection_to_current_layer_button);

        // Add to editor bottom
        positionable_selection_pane = memnew(editor::inspector::PositionableSelectionPane);
        positionable_pane_button = add_control_to_bottom_panel(positionable_selection_pane, POSITIONABLE_PANE_BUTTON_TITLE);
        positionable_pane_button->set_visible(false);

        // Add layers editor to dock
        layers_editor = memnew(editor::inspector::LayersEditor);

        RenderingServer::get_singleton()->connect("frame_post_draw", Callable(this, "_on_frame_post_draw"));
    }

    if (p_notification == NOTIFICATION_EXIT_TREE) {
        drop();
        memdelete(layers_editor);
    }
}

void IsometricEditorPlugin::edit(Object* p_object) {
    if (!p_object) {
        return;
    }

    auto map{cast_to<node::IsometricMap>(p_object)};
    EditorUndoRedoManager* undo_redo {get_undo_redo()};
    undo_redo->create_action("Edit IsometricMap", UndoRedo::MERGE_ALL, map);
    undo_redo->add_do_method(
      this,
      "_editp",
      get_tree()->get_edited_scene_root()->get_path().rel_path_to(map->get_path())
    );
    undo_redo->commit_action();
}

void IsometricEditorPlugin::_editp(const NodePath& p_path) {
    selected_map = cast_to<node::IsometricMap>(get_tree()->get_edited_scene_root()->get_node(p_path));
    String debug {p_path};

    if (!selected_map->is_connected("draw", Callable(this, "refresh"))) {
        selected_map->connect("draw", Callable(this, "refresh").bind(EditorPlane::PlaneType::EDITOR_DRAWER));
    }
    positionable_selection_pane->refresh_path_selector();
    positionable_selection_pane->set_positionable_set(selected_map->get_positionable_set());
    if (!selected_map->is_connected("positional_set_changed", Callable(positionable_selection_pane, "set_positionable_set"))) {
        selected_map->connect("positional_set_changed", Callable(positionable_selection_pane, "set_positionable_set"));
    }

    if (!selected_map->is_connected(IsometricStringNames::get_singleton()->size_changed_signal, Callable(this, "_on_map_size_changed"))) {
        selected_map->connect(IsometricStringNames::get_singleton()->size_changed_signal, Callable(this, "_on_map_size_changed"));
    }

    if (!handling_data_map.has(selected_map)) { handling_data_map[selected_map] = MapHandlingData(selected_map->get_size()); }
    editor::OutlineDrawer::set_outline_visible(selected_map, show_debug);

    _draw_edition_grid();

    editor::PositionableSelectorManager::get_instance().refresh_outline_for_selected(selected_map);

    layers_editor->refresh();
    if (!layers_editor->get_parent()) {
        add_control_to_dock(DockSlot::DOCK_SLOT_RIGHT_BL, layers_editor);
    }
}

void IsometricEditorPlugin::drop() {
    if (selected_map) {
        editor::OutlineDrawer::set_outline_visible(selected_map, false);
        if (selected_map->is_connected("draw", Callable(this, "refresh"))) {
            selected_map->disconnect("draw", Callable(this, "refresh"));
        }
        const MapHandlingData& map_handling_data = handling_data_map[selected_map];

        for (int i = EditorPlane::PlaneType::EDITOR_DRAWER; i < EditorPlane::PlaneType::SIZE; ++i) {
            EditionGridDrawer::clear_for_editor_plane(map_handling_data.editor_planes[i]);
        }
    }
    if (layers_editor->get_parent()) {
        remove_control_from_docks(layers_editor);
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

    Ref<InputEventKey> keyboard_event {p_event};
    if (keyboard_event.is_valid() && keyboard_event->is_pressed()) {
        Key key = keyboard_event->get_keycode_with_modifiers();
        if (keyboard_event->is_command_or_control_pressed()) {
            switch (key) {
                case Key::Z:
                    if (keyboard_event->is_shift_pressed()) {
                        EditorUndoRedoManager::get_singleton()->redo();
                    } else {
                        EditorUndoRedoManager::get_singleton()->undo();
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
            select_command_emitter.on_gui_input(p_event, selected_map);
            select_all_command_emitter.on_gui_input(p_event, selected_map);
            delete_command_emitter.on_gui_input(p_event, selected_map);
            move_selection_command_emitter.on_gui_input(p_event, selected_map);
            break;
        case PAINT:
            painting_command_emitter.on_gui_input(p_event, selected_map);
            break;
        case DRAG_AND_DROP:
            drag_and_drop_command_emitter.on_gui_input(p_event, selected_map);
            break;
    }
    move_editor_drawer_command_emitter.on_gui_input(p_event, selected_map);
    rotate_editor_plane_command_emitter.on_gui_input(p_event, selected_map);

    plane_view_limiter_command_emitter.on_gui_input(p_event, selected_map);
    return true;
}

void IsometricEditorPlugin::make_visible(bool b) {
    if (!b) { drop(); }
    toolbar->set_visible(b);
    positionable_pane_button->set_visible(b);
}

void IsometricEditorPlugin::refresh(int p_plane_type) {
    if (!handling_data_map.has(selected_map)) { return; }

    MapHandlingData& map_handling_data = handling_data_map[selected_map];
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

EditorPlane& IsometricEditorPlugin::get_editor_plane_for_map(node::IsometricMap* p_map, EditorPlane::PlaneType p_plane_type) {
    return handling_data_map[p_map].editor_planes[p_plane_type];
}

bool IsometricEditorPlugin::is_aabb_in_view_limiters(const AABB& p_aabb) const {
    const MapHandlingData& map_handling_data {handling_data_map[selected_map]};
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

uint32_t IsometricEditorPlugin::get_selected_layer() const {
    if (selected_map) {
        return selected_map->get_meta(node::IsometricMap::LAST_EDITED_LAYER_META_NAME, node::IsometricMap::DEFAULT_LAYER_ID);
    }

    return node::IsometricMap::DEFAULT_LAYER_ID;
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
    MapHandlingData& map_handling_data {handling_data_map[selected_map]};
    EditionGridDrawer::clear_for_editor_plane(map_handling_data.editor_planes[p_plane_type]);
    Ref<SceneTreeTimer> timer = map_handling_data.plane_timers[p_plane_type];
    if (!timer.is_valid()) { return; }
    map_handling_data.plane_timers[p_plane_type] = Ref<SceneTreeTimer>();
}

void IsometricEditorPlugin::_on_map_size_changed() {
    refresh(EditorPlane::PlaneType::EDITOR_DRAWER);
}

void IsometricEditorPlugin::_on_grid_color_picker_change([[maybe_unused]] const Color& p_color) {
    _draw_edition_grid();
}

void IsometricEditorPlugin::_on_move_selection_to_current_layer() {
    if (!selected_map) {
        return;
    }

    uint32_t current_layer {selected_map->get_meta(node::IsometricMap::LAST_EDITED_LAYER_META_NAME)};
    const Vector<Vector3>& selected_positions { PositionableSelectorManager::get_instance().get_selected_for_map(selected_map) };

    Vector<Ref<commands::Command<node::IsometricMap>>> commands;
    for (const Vector3& position : selected_positions) {
        uint32_t layer_id_at_position {selected_map->get_layer_id_at(position)};

        if (current_layer == layer_id_at_position) {
            continue;
        }

        if (node::IsometricPositionable* positionable {selected_map->get_positionable_at(position)}) {
            Ref<commands::SelectPositionableCommand> select_command_to_revert;
            select_command_to_revert.instantiate();
            select_command_to_revert->set_position(position);

            Ref<commands::RevertCommand<node::IsometricMap>> deselect_command;
            deselect_command.instantiate();
            deselect_command->set_reverse_command(select_command_to_revert);

            commands.push_back(deselect_command);

            Ref<commands::MoveToLayerCommand> move_to_layer_command;
            move_to_layer_command.instantiate();
            move_to_layer_command->set_position(position);
            move_to_layer_command->set_new_layer_id(current_layer);
            move_to_layer_command->set_former_layer_id(layer_id_at_position);
            commands.push_back(move_to_layer_command);

            Ref<commands::SelectPositionableCommand> select_command;
            select_command.instantiate();
            select_command->set_position(position);
            commands.push_back(select_command);
        }
    }

    if (commands.is_empty()) {
        return;
    }

    commands::emitters::CommandToActionTransformer action_transformer;
    action_transformer.transform<node::IsometricMap, MOVE_TO_CURRENT_LAYER_COMMAND, UndoRedo::MERGE_DISABLE, true>(commands, selected_map);
}

void IsometricEditorPlugin::_draw_edition_grid() const {
    const MapHandlingData& map_handling_data {handling_data_map[selected_map]};
    EditionGridDrawer::draw_grid(
      map_handling_data.editor_planes[EditorPlane::PlaneType::EDITOR_DRAWER],
      selected_map,
      grid_color_picker_button->get_pick_color()
    );
}

void IsometricEditorPlugin::_draw_plane(EditorPlane::PlaneType p_plane_type) {
    const EditorPlane& plane {handling_data_map[selected_map].editor_planes[p_plane_type]};
    EditionGridDrawer::draw_plane(plane, selected_map);
}

void IsometricEditorPlugin::_set_plane_timer(EditorPlane::PlaneType p_plane_type, float p_delay) {
    static const StringName timeoutSignalName {"timeout"};

    MapHandlingData& map_handling_data {handling_data_map[selected_map]};

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
    ClassDB::bind_method(D_METHOD("_editp", "p_path"), &IsometricEditorPlugin::_editp);
}

#endif
