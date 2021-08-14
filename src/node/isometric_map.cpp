#include "isometric_map.h"

using namespace node;

IsometricMap::IsometricMap() :
        IsometricPositionable(), draw_tiles(true), grid_3d(), instances_grid_3d(), positionable_set() {
}

Ref<resource::PositionableSet> IsometricMap::get_positionable_set() const {
    return positionable_set;
}

void IsometricMap::set_positionable_set(const Ref<resource::PositionableSet>& set) {
    positionable_set = set;
    if (!positionable_set->is_set_loaded()) {
        positionable_set->preload_scenes();
    }
    emit_signal("positional_set_changed", positionable_set);
}

#ifdef TOOLS_ENABLED

void IsometricMap::add_positionable_if_nothing_present(const AABB& aabb, int id) {
    if (instances_grid_3d.is_overlapping(aabb)) return;

    const Vector3& position{aabb.position};
    grid_3d.set_data(position, id);
    add_positionable_as_child(id, position);
}

void IsometricMap::remove_positionable(const AABB& aabb) {
    grid_3d.set_data(aabb.position, containers::Grid3D<int, resource::PositionableSet::NONE_POSITIONABLE_ID>::get_default_value());
    instances_grid_3d.insert_box(aabb, nullptr, true);
}

Object* IsometricMap::get_positionable_at(const Vector3& position) {
    return instances_grid_3d.get_data(position);
}

bool IsometricMap::is_aabb_in_map(const AABB& aabb) const {
    const Vector3& position{aabb.position};
    const Vector3& size{aabb.size};
    int pos_x{static_cast<int>(position.x)};
    int pos_y{static_cast<int>(position.y)};
    int pos_z{static_cast<int>(position.z)};
    int max_x{pos_x + static_cast<int>(size.x) - 1};
    int max_y{pos_y + static_cast<int>(size.y) - 1};
    int max_z{pos_z + static_cast<int>(size.z) - 1};

    if (pos_x < 0 || pos_y < 0 || pos_z < 0 ||
        max_x >= grid_3d.get_width() || max_y >= grid_3d.get_depth() || max_z >= grid_3d.get_height()) {
        return false;
    }
    return true;
}

void IsometricMap::set_size(Vector3 p_size) {
    IsometricPositionable::set_size(p_size);
    grid_3d.update_array_size(p_size);
    instances_grid_3d.update_array_size(p_size);
}

bool IsometricMap::is_overlapping(const AABB& aabb) const {
    return instances_grid_3d.is_overlapping(aabb);
}

#endif

void IsometricMap::_notification(int notif) {
    if (notif == NOTIFICATION_ENTER_TREE) {
        _on_enter_tree();
    }
}

void IsometricMap::_on_enter_tree() {
    const Vector<int>& id_vector{grid_3d.get_internal_array()};
    for (int i = 0; i < id_vector.size(); ++i) {
        add_positionable_as_child(id_vector[i], grid_3d.get_position_3d_from_index(i));
    }
}

Array IsometricMap::_get_grid_3d() const {
    const Vector<int>& grid_internal_array{grid_3d.get_internal_array()};
    Array ret;
    for (int i = 0; i < grid_internal_array.size(); ++i) {
        ret.append(grid_internal_array[i]);
    }
    return ret;
}

void IsometricMap::_set_grid_3d(const Array& array) {
    Vector<int> internal_array;
    for (int i = 0; i < array.size(); ++i) {
        internal_array.push_back(array[i]);
    }
    grid_3d.set_internal_array(internal_array);
}

void IsometricMap::add_positionable_as_child(int positionable_id, const Vector3& position) {
    if (positionable_id == resource::PositionableSet::NONE_POSITIONABLE_ID) return;
    if (auto* positionable{
            Object::cast_to<IsometricPositionable>(
                    positionable_set->get_positionable_scene_for_id(positionable_id)->instance()
            )
        }
    ) {
        add_child(positionable);
        positionable->set_local_position_3d(position);

        instances_grid_3d.insert_box({position, positionable->get_size()}, positionable);
    }
}

void IsometricMap::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_positionable_set"), &IsometricMap::get_positionable_set);
    ClassDB::bind_method(D_METHOD("set_positionable_set"), &IsometricMap::set_positionable_set);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "positionable_set"), "set_positionable_set", "get_positionable_set");

    ClassDB::bind_method(D_METHOD("_get_grid_3d"), &IsometricMap::_get_grid_3d);
    ClassDB::bind_method(D_METHOD("_set_grid_3d"), &IsometricMap::_set_grid_3d);
    ADD_PROPERTY(
            PropertyInfo(Variant::ARRAY, "grid_3d", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR | PROPERTY_USAGE_INTERNAL),
            "_set_grid_3d",
            "_get_grid_3d"
    );

    ADD_SIGNAL(MethodInfo("positional_set_changed", PropertyInfo(Variant::OBJECT, "set", PROPERTY_HINT_RESOURCE_TYPE, "PositionalSet")));
}