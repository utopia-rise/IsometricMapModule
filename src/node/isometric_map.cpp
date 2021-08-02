#include "isometric_map.h"

using namespace node;

IsometricMap::IsometricMap() :
        IsometricPositionable(), draw_tiles(true), grid_3d(), positionable_set() {
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

void IsometricMap::add_positionable_if_nothing_present(const Vector3& position, int id) {
    if (grid_3d.get_data(position) != containers::Grid3D<int, -1>::get_default_value()) return;

    grid_3d.set_data(position, id);
}

void IsometricMap::remove_positionable(const Vector3& position) {
    grid_3d.set_data(position, containers::Grid3D<int, -1>::get_default_value());
}

void IsometricMap::_notification(int notif) {
    if (notif == NOTIFICATION_ENTER_TREE) {
        _on_enter_tree();
    }
}

void IsometricMap::_on_enter_tree() {
    const Vector<int>& id_vector{grid_3d.get_internal_array()};
    for (int i = 0; i < id_vector.size(); ++i) {
        add_child(positionable_set->get_positionable_scene_for_id(id_vector[i])->instance());
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