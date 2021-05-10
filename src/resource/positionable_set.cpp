#include "positionable_set.h"
#include <modules/isometric_maps/src/node/isometric_positionable.h>
#include <modules/isometric_maps/src/node/isometric_map.h>
#include <core/os/file_access.h>
#include <core/os/dir_access.h>
#include <core/io/resource_saver.h>

using namespace resource;

const PoolStringArray& PositionableSet::get_positionable_paths() const {
    return positionable_paths;
}

void PositionableSet::set_positionable_paths(const PoolStringArray& paths) {
    positionable_paths = paths;
    refresh_set();
}

Error PositionableSet::refresh_set() {
    for (int i = 0; i < positionable_paths.size(); ++i) {
        String path{positionable_paths.get(i)};
        if (path.empty() || path == "res://") {
            return Error::ERR_CANT_RESOLVE;
        }
        if (!path.begins_with("res://")) {
            path = vformat("res://%s", path);
        }
        if (insert_all_positionables_for_path(path) != Error::OK) {
            return Error::ERR_CANT_RESOLVE;
        }
    }
    List<StringName> keys;
    scene_sets.get_key_list(&keys);
    for (int i = 0; i < keys.size(); ++i) {
        bool contained;
        StringName& hash{keys[i]};
        for (int j = 0; j < positionable_paths.size(); ++j) {
            if (hash == positionable_paths[j]) {
                contained = true;
            }
        }
        if (!contained) {
            scene_sets.erase(hash);
        }
    }

    return Error::OK;
}

Error PositionableSet::insert_all_positionables_for_path(const String& path) {
    StringName hash{path};
    if (scene_sets.has(hash)) {
        return Error::OK;
    }

    Error error;
    DirAccess* dir_access{DirAccess::open(path, &error)};

    if (error != OK) {
        FileAccess* file_access{FileAccess::open(path, FileAccess::READ, &error)};
        if (error != OK) {
            //TODO : show popup saying wrong path
            WARN_PRINT(vformat("%s cannot be opened", path))
            return Error::ERR_CANT_RESOLVE;
        }
        if (path.ends_with(".tscn")) {
            insert_scene_if_positionable(hash);
        }
        file_access->close();
        memdelete(file_access);
    } else {
        dir_access->list_dir_begin();
        String item;
        while (!(item = dir_access->get_next()).empty()) {
            if (item == "." || item == "..") {
                continue;
            }
            insert_all_positionables_for_path(path.plus_file(item));
        }
        memdelete(dir_access);
    }
    return Error::OK;
}

void PositionableSet::insert_scene_if_positionable(const StringName& path) {
    RES resource{ResourceLoader::load(path)};
    if (auto* packed_scene{Object::cast_to<PackedScene>(resource.ptr())}) {
        if (auto* positionable{Object::cast_to<node::IsometricPositionable>(packed_scene->instance())}) {
            if (!scene_sets.has(path)) {
                scene_sets[path] = {Vector<Ref<PackedScene>>(), Vector<Ref<PackedScene>>()};
            }
            if (Object::cast_to<node::IsometricMap>(positionable)) {
                scene_sets[path].maps.push_back(Ref<PackedScene>(packed_scene));
            } else {
                scene_sets[path].positionables.push_back(Ref<PackedScene>(packed_scene));
            }
            WARN_PRINT(vformat("Inserted %s", path))
            memdelete(positionable);
        }
    }
}

PositionableSet::PositionableSet() : positionable_paths(), scene_sets() {

}

void PositionableSet::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_positionable_paths", "paths"), &PositionableSet::set_positionable_paths);
    ClassDB::bind_method(D_METHOD("get_positionable_paths"), &PositionableSet::get_positionable_paths);
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "positionable_paths"), "set_positionable_paths", "get_positionable_paths");
    ADD_PROPERTY_DEFAULT("positionable_paths", PoolStringArray());
}
