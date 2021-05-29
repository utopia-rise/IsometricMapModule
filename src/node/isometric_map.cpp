#include "isometric_map.h"

using namespace node;

IsometricMap::IsometricMap() :
        IsometricPositionable(), draw_tiles(true) {
}

void IsometricMap::_ready() {
    for (int i = 0; i < get_child_count(); i++) {
        auto* positionable = cast_to<IsometricPositionable>(get_child(i));
        if (positionable) {
            grid_3d.set_data(positionable->get_local_position_3d(), positionable);
            edition_grid_3d.insert_box(AABB(positionable->get_local_position_3d(), positionable->get_size()),
                                       positionable);
        }
    }
}

void IsometricMap::add_iso_positionable(Node* node) {
    auto isometric_positionable{cast_to<IsometricPositionable>(node)};
    const Vector3 &map_size{get_size()};
    const AABB &aabb{AABB(isometric_positionable->get_local_position_3d(), isometric_positionable->get_size())};
    const Vector3 &pos{aabb.position};
    bool is_overlapping;
    auto* child_map{cast_to<IsometricMap>(isometric_positionable)};
    if (child_map) {
        is_overlapping = are_map_elements_overlapping(pos, child_map);
    } else {
        is_overlapping = edition_grid_3d.is_overlapping(aabb);
    }
    if (pos.x >= map_size.x || pos.y >= map_size.y || pos.z >= map_size.z || is_overlapping) return;

    grid_3d.set_data(aabb.position, isometric_positionable);

    if (child_map) {
        insert_map_as_flatten(child_map, pos);
    } else {
        edition_grid_3d.insert_box(aabb, isometric_positionable);
    }
    add_child(isometric_positionable);
}

void IsometricMap::remove_iso_positionable(Node* node) {
    auto isometric_positionable{cast_to<IsometricPositionable>(node)};
    const Vector3 &map_size{get_size()};
    const AABB &aabb{AABB(isometric_positionable->get_local_position_3d(), isometric_positionable->get_size())};
    const Vector3 &pos{aabb.position};
    if (pos.x >= map_size.x || pos.y >= map_size.y || pos.z >= map_size.z) return;
    remove_child(isometric_positionable);

    grid_3d.set_data(aabb.position, nullptr);

    if (auto* child_map{cast_to<IsometricMap>(isometric_positionable)}) {
        const Array &child_map_children{child_map->get_flatten_positionables(pos)};
        for (int i = 0; i < child_map_children.size(); i++) {
            if (auto* posi{cast_to<IsometricPositionable>(child_map_children[i])}) {
                edition_grid_3d.insert_box(AABB(posi->get_local_position_3d(), posi->get_size()), child_map, true);
            }
        }
    } else {
        edition_grid_3d.insert_box(aabb, isometric_positionable, true);
    }
    isometric_positionable->update();
}

Node* IsometricMap::get_positionable_at(Vector3 pos, bool only_left_upper_corner) {
    return cast_to<IsometricPositionable>(
            only_left_upper_corner ? grid_3d.get_data(pos) : edition_grid_3d.get_data(pos));
}

bool IsometricMap::is_overlapping(Node* node) {
    auto isometric_positionable{cast_to<IsometricPositionable>(node)};
    return edition_grid_3d.is_overlapping(
            AABB(isometric_positionable->get_local_position_3d(), isometric_positionable->get_size()));
}

bool IsometricMap::is_overlapping_aabb(AABB aabb) {
    return edition_grid_3d.is_overlapping(aabb);
}

bool IsometricMap::are_map_elements_overlapping(Vector3 position, Node* obj) {
    auto map{cast_to<IsometricMap>(obj)};
    for (int i = 0; i < map->get_child_count(); i++) {
        if (auto* positionable{cast_to<IsometricPositionable>(map->get_child(i))}) {
            if (auto* child_map{cast_to<IsometricMap>(positionable)}) {
                if (are_map_elements_overlapping(position + child_map->get_local_position_3d(), child_map)) {
                    return true;
                }
            } else {
                if (is_overlapping_aabb(
                        {position + positionable->get_local_position_3d(), positionable->get_size()})) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool IsometricMap::has(Node* isometric_positionable) {
    return grid_3d.has(isometric_positionable);
}

Array IsometricMap::get_positionable_children() const {
    Array positionable_children;
    const Vector<Object*> &grid_array = grid_3d.get_internal_array();
    for (int i = 0; i < grid_array.size(); i++) {
        const Variant &element = grid_array[i];
        if (element) {
            positionable_children.append(element);
        }
    }
    return positionable_children;
}


Array IsometricMap::get_flatten_positionables(const Vector3 &offset) {
    Array positionables;
    for (int i = 0; i < get_child_count(); i++) {
        auto* positionable = cast_to<IsometricPositionable>(get_child(i));
        if (positionable) {
            if (auto* map = cast_to<IsometricMap>(positionable)) {
                const Array &inner_positionables{map->get_flatten_positionables(offset + map->get_local_position_3d())};
                for (int j = 0; j < inner_positionables.size(); j++) {
                    if (auto* inner_positionable{cast_to<IsometricPositionable>(inner_positionables[j])}) {
                        positionables.append(inner_positionable);
                    }
                }
            } else {
                auto* duplicate_positionable = cast_to<IsometricPositionable>(positionable->duplicate());
                duplicate_positionable->set_local_position_3d(duplicate_positionable->get_local_position_3d() + offset);
                positionables.append(duplicate_positionable);
            }
        }
    }
    return positionables;
}

void IsometricMap::insert_map_as_flatten(Node* map, const Vector3 &offset) {
    for (int i = 0; i < get_child_count(); i++) {
        if (auto* positionable{cast_to<IsometricPositionable>(get_child(i))}) {
            if (auto* m{cast_to<IsometricMap>(positionable)}) {
                insert_map_as_flatten(m, offset + m->get_local_position_3d());
            } else {
                const AABB &aabb{positionable->get_local_position_3d() + offset, positionable->get_size()};
                edition_grid_3d.insert_box(aabb, map);
            }
        }
    }
}

void IsometricMap::_bind_methods() {
    ClassDB::bind_method(D_METHOD("add_iso_positionable"), &IsometricMap::add_iso_positionable);
    ClassDB::bind_method(D_METHOD("remove_iso_positionable"), &IsometricMap::remove_iso_positionable);
    ClassDB::bind_method(D_METHOD("get_positionable_at"), &IsometricMap::get_positionable_at);
    ClassDB::bind_method(D_METHOD("is_overlapping"), &IsometricMap::is_overlapping);
    ClassDB::bind_method(D_METHOD("is_overlapping_aabb"), &IsometricMap::is_overlapping_aabb);
    ClassDB::bind_method(D_METHOD("are_map_elements_overlapping"), &IsometricMap::are_map_elements_overlapping);
    ClassDB::bind_method(D_METHOD("has", "isometric_positionable"), &IsometricMap::has);
    ClassDB::bind_method(D_METHOD("get_positionable_children"), &IsometricMap::get_positionable_children);
}

void IsometricMap::_notification(int notif) {
    switch (notif) {
        case NOTIFICATION_READY:
            _ready();
            break;
        default:
            break;
    }
}
