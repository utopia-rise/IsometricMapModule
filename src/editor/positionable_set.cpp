#ifdef TOOLS_ENABLED

#include "positionable_set.h"

using namespace editor;

const PoolStringArray& PositionableSet::get_positionable_paths() const {
    return positionable_path;
}

void PositionableSet::set_positionable_paths(const PoolStringArray& paths) {
    positionable_path = paths;
}

void PositionableSet::refresh_set() {
    for (int i = 0; i < positionable_path.size(); ++i) {
        String current_path{positionable_path.get(i)};
        if (!current_path.begins_with("res://")) {
            current_path = vformat("res://%s", current_path);
        }
        StringName current_path_hash{current_path};
        if (scene_sets.has(current_path_hash)) {
            continue;
        }
        if (current_path.ends_with(".tscn")) {
            insert_scene_if_positionable(current_path_hash);
        }
        //TODO: check if folder and loop over contained scenes.
    }
}

void PositionableSet::insert_scene_if_positionable(const StringName& path) {
    if (!scene_sets.has(path)) {
        scene_sets[path] = Vector<PackedScene>();
    }
    //TODO: load positionable scene and fill scene_sets
}

PositionableSet::PositionableSet() : positionable_path(), scene_sets() {

}

void PositionableSet::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_positionable_paths", "paths"), &PositionableSet::set_positionable_paths);
    ClassDB::bind_method(D_METHOD("get_positionable_paths"), &PositionableSet::get_positionable_paths);
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "positionable_paths"), "set_positionable_paths", "get_positionable_paths");
    ADD_PROPERTY_DEFAULT("positionable_paths", PoolStringArray());
}

#endif
