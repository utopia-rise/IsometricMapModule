#include "positionable_set.h"
#include <modules/isometric_maps/src/node/isometric_positionable.h>
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

const Vector<Ref<PackedScene>>& PositionableSet::get_storage_for_path(const StringName &path) {
    return scenes_storage_map[path];
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
        if (insert_all_positionables_for_path(path, nullptr) != Error::OK) {
            return Error::ERR_CANT_RESOLVE;
        }
    }
    List<StringName> keys;
    scenes_storage_map.get_key_list(&keys);
    for (int i = 0; i < keys.size(); ++i) {
        bool contained;
        StringName& hash{keys[i]};
        for (int j = 0; j < positionable_paths.size(); ++j) {
            if (hash == positionable_paths[j]) {
                contained = true;
            }
        }
        if (!contained) {
            scenes_storage_map.erase(hash);
        }
    }

    emit_changed();
    return Error::OK;
}

Error PositionableSet::insert_all_positionables_for_path(const String& path, const char* base_path) {
    StringName path_entry;
    if (!base_path) {
        path_entry = path;
        if (scenes_storage_map.has(path_entry)) {
            scenes_storage_map[path_entry].clear();
        }
    } else {
        path_entry = base_path;
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
            insert_scene_if_positionable(path_entry, path);
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
            insert_all_positionables_for_path(path.plus_file(item), path_entry.operator String().utf8());
        }
        memdelete(dir_access);
    }
    return Error::OK;
}

void PositionableSet::insert_scene_if_positionable(const StringName& base_path, const String& resource_path) {
    RES resource{ResourceLoader::load(resource_path)};
    if (auto* packed_scene{Object::cast_to<PackedScene>(resource.ptr())}) {
        //TODO: investigate get_inheritance_list_static generated from GDClass
        if (auto* positionable{Object::cast_to<node::IsometricPositionable>(packed_scene->instance())}) {
            if (!scenes_storage_map.has(base_path)) {
                scenes_storage_map[base_path] = Vector<Ref<PackedScene>>();
            }
            scenes_storage_map[base_path].push_back(Ref<PackedScene>(packed_scene));
            memdelete(positionable);
        }
    }
}

PositionableSet::PositionableSet() : positionable_paths(), scenes_storage_map() {

}

void PositionableSet::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_positionable_paths", "paths"), &PositionableSet::set_positionable_paths);
    ClassDB::bind_method(D_METHOD("get_positionable_paths"), &PositionableSet::get_positionable_paths);
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "positionable_paths"), "set_positionable_paths", "get_positionable_paths");
    ADD_PROPERTY_DEFAULT("positionable_paths", PoolStringArray());
}
