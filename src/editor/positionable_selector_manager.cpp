#ifdef TOOLS_ENABLED

#include "positionable_selector_manager.h"

using namespace editor;

PositionableSelectorManager& PositionableSelectorManager::get_instance() {
    static PositionableSelectorManager instance;
    return instance;
}

void
PositionableSelectorManager::select_positionable(node::IsometricMap* map, node::IsometricPositionable* positionable) {
    if (!map_to_selected_positionables.has(map)) {
        map_to_selected_positionables[map] = containers::Grid3D<node::IsometricPositionable*, nullptr>();
    }
    map_to_selected_positionables[map].update_array_size(map->get_size());
    show_outline(positionable);
    map_to_selected_positionables[map].insert_box(
            {positionable->get_local_position_3d(), positionable->get_size()},
            positionable
    );
}

void PositionableSelectorManager::deselect_positionable_at(node::IsometricMap* map, const Vector3& position) {
    containers::Grid3D<node::IsometricPositionable*, nullptr>& selected_positionables{
        map_to_selected_positionables[map]
    };
    node::IsometricPositionable* selected{selected_positionables.get_data(position)};
    if (!selected) {
        return;
    }
    selected->show_outline(false);
    selected_positionables.insert_box({position, selected->get_size()}, nullptr, true);
}

void PositionableSelectorManager::deselect_all(node::IsometricMap* map) {
    containers::Grid3D<node::IsometricPositionable*, nullptr>& selected_positionables{
        map_to_selected_positionables[map]
    };
    const Vector<node::IsometricPositionable*>& positionables{selected_positionables.get_internal_array()};
    for (int i = 0; i < positionables.size(); ++i) {
        if (node::IsometricPositionable* positionable{positionables[i]}) {
            positionable->show_outline(false);
        }
    }
    selected_positionables.update_array_size(selected_positionables.get_dimensions(), true);
}

const containers::Grid3D<node::IsometricPositionable*, nullptr>&
PositionableSelectorManager::get_selected_for_map(node::IsometricMap* map) {
    return map_to_selected_positionables[map];
}

void PositionableSelectorManager::set_selected_for_map(node::IsometricMap* map,
                                                       const containers::Grid3D<node::IsometricPositionable*, nullptr>& selected) {
    map_to_selected_positionables[map] = selected;
    const Vector<node::IsometricPositionable*>& selected_array{selected.get_internal_array()};
    for (int i = 0; i < selected_array.size(); ++i) {
        node::IsometricPositionable* positionable{selected_array[i]};
        if (!positionable) {
            continue;
        }
        show_outline(positionable);
    }
}

void PositionableSelectorManager::show_outline(node::IsometricPositionable* positionable) {
    positionable->set_outline_drawer(Color(1, 0, 0, 1), 10, false);
    positionable->show_outline(true);
}

PositionableSelectorManager::PositionableSelectorManager() : map_to_selected_positionables() {

}

#endif