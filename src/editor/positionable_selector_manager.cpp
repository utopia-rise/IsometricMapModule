#ifdef TOOLS_ENABLED

    #include "positionable_selector_manager.h"
    #include "outline_drawer.h"

using namespace editor;

PositionableSelectorManager& PositionableSelectorManager::get_instance() {
    static PositionableSelectorManager instance;
    return instance;
}

void PositionableSelectorManager::select_positionable_at(node::IsometricMap* map, node::IsometricPositionable* positionable) {
    if (!map_to_selected_positions.has(map)) { map_to_selected_positions[map] = Vector<Vector3>(); }
    show_outline(positionable);
    map_to_selected_positions[map].push_back(positionable->get_local_position_3d());
}

void PositionableSelectorManager::deselect_positionable_at(node::IsometricMap* map, const Vector3& position) {
    Vector<Vector3>& selected_positions {map_to_selected_positions[map]};
    #ifdef TOOLS_ENABLED
    node::IsometricPositionable* selected {map->get_positionable_at(position)};
    if (!selected) { return; }
    editor::OutlineDrawer::set_outline_visible(selected, false);
    #endif
    selected_positions.erase(position);
}

void PositionableSelectorManager::deselect_all(node::IsometricMap* map) {
    Vector<Vector3>& selected_positions {map_to_selected_positions[map]};
    #ifdef TOOLS_ENABLED
    for (int i = 0; i < selected_positions.size(); ++i) {
        if (node::IsometricPositionable * positionable {map->get_positionable_at(selected_positions[i])}) {
            editor::OutlineDrawer::set_outline_visible(positionable, false);
        }
    }
    #endif
    selected_positions.clear();
}

const Vector<Vector3>& PositionableSelectorManager::get_selected_for_map(node::IsometricMap* map) {
    return map_to_selected_positions[map];
}

void PositionableSelectorManager::set_selected_for_map(node::IsometricMap* map, const Vector<Vector3>& selected) {
    map_to_selected_positions[map] = selected;
    for (int i = 0; i < selected.size(); ++i) {
        node::IsometricPositionable* positionable {map->get_positionable_at(selected[i])};
        if (!positionable) { continue; }
        show_outline(positionable);
    }
}

bool PositionableSelectorManager::is_position_selected_for_map(node::IsometricMap* map, const Vector3& p_position) {
    return map_to_selected_positions[map].find(p_position) > -1;
}

void PositionableSelectorManager::refresh_outline_for_selected(node::IsometricMap* map) {
    const Vector<Vector3>& selected {map_to_selected_positions[map]};
    for (int i = 0; i < selected.size(); ++i) {
        if (node::IsometricPositionable * positionable {map->get_positionable_at(selected[i])}) { show_outline(positionable); }
    }
}

void PositionableSelectorManager::show_outline(node::IsometricPositionable* positionable) {
    OutlineData& outline_data {positionable->get_outline_data()};
    outline_data.color = {1, 0, 0, 1};
    outline_data.line_size = 10;
    editor::OutlineDrawer::draw_outline(positionable);
    editor::OutlineDrawer::set_outline_visible(positionable, true);
}

PositionableSelectorManager::PositionableSelectorManager() : map_to_selected_positions() {}

#endif