#include "positionable_set.h"
#include <modules/isometric_maps/src/node/isometric_positionable.h>

#ifdef TOOLS_ENABLED
#include <core/os/file_access.h>
#include <core/os/dir_access.h>
#include <core/io/resource_saver.h>
#endif

using namespace resource;

void PositionableSet::get_positionable_scene_path_for_id(int id, String& r_path) const {
    r_path = identifier_to_scene_path[id];
}

#ifdef TOOLS_ENABLED

const PoolStringArray& PositionableSet::get_path_groups() const {
    return path_groups;
}

void PositionableSet::set_path_groups(const PoolStringArray& paths) {
    path_groups = paths;

    if (unlikely(!(editor_check_set_call & PATH_GROUPS))) {
        editor_check_set_call &= PATH_GROUPS;
    }
    if (is_data_set()) {
        refresh_set();
    }
}

Vector<String> PositionableSet::get_scene_paths_for_group(const String& p_group) const {
    const PoolVector<int>& ids{group_to_identifiers[p_group]};

    Vector<String> paths_for_group;
    for (int i = 0; i < ids.size(); ++i) {
        const Variant& path{identifier_to_scene_path[ids[i]]};
        if (path.get_type() == Variant::STRING) {
            paths_for_group.push_back(path);
        }
    }
    return paths_for_group;
}

Error PositionableSet::refresh_set() {
    for (int i = 0; i < path_groups.size(); ++i) {
        String path{path_groups.get(i)};
        if (path.empty() || path == "res://") {
            return Error::ERR_CANT_RESOLVE;
        }
        if (!path.begins_with("res://")) {
            path = vformat("res://%s", path);
        }
        if (insert_all_positionables_for_path_if_not_present(path, nullptr) != Error::OK) {
            return Error::ERR_CANT_RESOLVE;
        }
    }

    const Array& values{identifier_to_scene_path.values()};
    Vector<int> ids_to_remove;
    for (int i = 0; i < values.size(); ++i) {
        const String& scene_path{values[i]};
        FileAccessRef file_access{FileAccess::create(FileAccess::ACCESS_RESOURCES)};
        if (!file_access->file_exists(scene_path)) {
            ids_to_remove.push_back(identifier_to_scene_path.keys()[i]);
        }
    }

    const Array& keys{group_to_identifiers.keys()};
    for (int i = 0; i < keys.size(); ++i) {
        bool contained;
        for (int j = 0; j < path_groups.size(); ++j) {
            if (path_groups[j] == keys[i]) {
                contained = true;
                break;
            }
        }
        if (!contained) {
            ids_to_remove.append_array(group_to_identifiers.values()[i]);
        }
    }
    
    for (int i = 0; i < ids_to_remove.size(); ++i) {
        int id{ids_to_remove[i]};
        removed_elements.push_back({id, identifier_to_scene_path[id]});
        identifier_to_scene_path.erase(id);
    }
    
    emit_changed();
    return Error::OK;
}

const Vector<RemovedSetElement>& PositionableSet::get_removed_elements() const {
    return removed_elements;
}

void PositionableSet::insert_path_at_id(const String& existing_group_path, int id, const String& scene_path) {
    PoolVector<int> identifiers{group_to_identifiers[existing_group_path]};
    identifiers.push_back(id);
    identifier_to_scene_path[id] = scene_path;
}

Error PositionableSet::insert_all_positionables_for_path_if_not_present(const String& path, const char* base_path) {
    StringName path_entry;
    if (!base_path) {
        path_entry = path;
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
            insert_positionable_scene_if_not_present(path_entry, path);
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
            insert_all_positionables_for_path_if_not_present(path.plus_file(item), path_entry.operator String().utf8());
        }
        memdelete(dir_access);
    }
    return Error::OK;
}

void PositionableSet::insert_positionable_scene_if_not_present(const String& path_group, const String& resource_path) {
    RES resource{ResourceLoader::load(resource_path)};
    if (auto* packed_scene{Object::cast_to<PackedScene>(resource.ptr())}) {
        //TODO: investigate get_inheritance_list_static generated from GDClass
        if (auto* positionable{Object::cast_to<node::IsometricPositionable>(packed_scene->instance())}) {
            if (!identifier_to_scene_path.values().has(resource_path)) {
                identifier_to_scene_path[next_id] = resource_path;
                if (!group_to_identifiers.has(path_group)) {
                    group_to_identifiers[path_group] = PoolVector<int>();
                }
                PoolVector<int> identifiers{group_to_identifiers[path_group]};
                identifiers.push_back(next_id);
                ++next_id;
            }
            memdelete(positionable);
        }
    }
}

const Dictionary& PositionableSet::_get_group_to_identifiers() const {
    return group_to_identifiers;
}

void PositionableSet::_set_group_to_identifiers(const Dictionary& p_group_to_identifiers) {
    group_to_identifiers = p_group_to_identifiers;

    if (unlikely(!(editor_check_set_call & GROUP_TO_IDENTIFIER))) {
        editor_check_set_call &= GROUP_TO_IDENTIFIER;
    }
    if (is_data_set()) {
        refresh_set();
    }
}

bool PositionableSet::is_data_set() const {
    return likely(editor_check_set_call == (PATH_GROUPS | GROUP_TO_IDENTIFIER | IDENTIFIER_TO_SCENE_PATH));
}

#endif

const Dictionary& PositionableSet::_get_identifier_to_scene_path() const {
    return identifier_to_scene_path;
}

void PositionableSet::_set_identifier_to_scene_path(const Dictionary& p_identifier_to_scene_path) {
    identifier_to_scene_path = p_identifier_to_scene_path;

#ifdef TOOLS_ENABLED
    if (unlikely(!(editor_check_set_call & IDENTIFIER_TO_SCENE_PATH))) {
        editor_check_set_call &= IDENTIFIER_TO_SCENE_PATH;
        next_id = identifier_to_scene_path.size();
    }
    if (is_data_set()) {
        refresh_set();
    }
#endif
}

PositionableSet::PositionableSet() :
        identifier_to_scene_path()
#ifdef TOOLS_ENABLED
        , path_groups(),
        group_to_identifiers(),
        editor_check_set_call(),
        next_id(),
        removed_elements()
#endif
{

}

void PositionableSet::_bind_methods() {
#ifdef TOOLS_ENABLED
    ClassDB::bind_method(D_METHOD("set_path_groups", "paths"), &PositionableSet::set_path_groups);
    ClassDB::bind_method(D_METHOD("get_path_groups"), &PositionableSet::get_path_groups);
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "path_groups"), "set_path_groups", "get_path_groups");
    ADD_PROPERTY_DEFAULT("path_groups", PoolStringArray());

    ClassDB::bind_method(D_METHOD("_set_group_to_identifiers", "p_group_to_identifiers"),
                         &PositionableSet::_set_group_to_identifiers);
    ClassDB::bind_method(D_METHOD("_get_group_to_identifiers"), &PositionableSet::_get_group_to_identifiers);
    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "group_to_identifiers", PROPERTY_HINT_NONE, "",
                              PROPERTY_USAGE_NOEDITOR | PROPERTY_USAGE_INTERNAL), "_set_group_to_identifiers",
                 "_get_group_to_identifiers");
    ADD_PROPERTY_DEFAULT("group_to_identifiers", Dictionary());
#endif

    ClassDB::bind_method(D_METHOD("_set_identifier_to_scene_path", "identifier_to_scene_path"),
                         &PositionableSet::_set_identifier_to_scene_path);
    ClassDB::bind_method(D_METHOD("_get_identifier_to_scene_path", "identifier_to_scene_path"),
                         &PositionableSet::_get_identifier_to_scene_path);
    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "identifier_to_scene_path", PROPERTY_HINT_NONE, "",
                              PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_INTERNAL), "_set_identifier_to_scene_path",
                 "_get_identifier_to_scene_path");
    ADD_PROPERTY_DEFAULT("identifier_to_scene_path", Dictionary());
}
