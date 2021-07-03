#include "positionable_set.h"
#include <modules/isometric_maps/src/node/isometric_positionable.h>

#ifdef TOOLS_ENABLED
#include <core/os/file_access.h>
#include <core/os/dir_access.h>
#include <core/io/resource_saver.h>
#include <modules/isometric_maps/src/editor/editor_utils.h>

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
}

Map<int, String> PositionableSet::get_scene_paths_for_group(const String& p_group) const {
    if (!group_to_identifiers.has(p_group)) {
        return {};
    }

    const Vector<int>& ids{group_to_identifiers[p_group]};

    Map<int, String> paths_for_group;
    for (int i = 0; i < ids.size(); ++i) {
        const Variant& path{identifier_to_scene_path[ids[i]]};
        if (path.get_type() == Variant::STRING) {
            paths_for_group[ids[i]] = path;
        }
    }
    return paths_for_group;
}

Error PositionableSet::refresh_set() {
    remove_not_anymore_presents_groups_to_identifiers();

    for (int i = 0; i < path_groups.size(); ++i) {
        String path{path_groups.get(i)};
        if (path.empty() || path == "res://") {
            return Error::ERR_CANT_RESOLVE;
        }
        if (!path.begins_with("res://")) {
            path = vformat("res://%s", path);
        }
        List<String> positionable_paths;
        if (editor::EditorUtils::find_all_positionables_in_path(path, &positionable_paths)) {
            return Error::ERR_CANT_RESOLVE;
        }
        for (int j = 0; j < positionable_paths.size(); ++j) {
            insert_positionable_if_not_present(path, positionable_paths[j]);
        }
    }

    emit_changed();
    return Error::OK;
}

Vector<resource::PositionableSet::RemovedElement> PositionableSet::get_removed_elements() const {
    Vector<RemovedElement> removed_elements;

    List<StringName> keys;
    group_to_identifiers.get_key_list(&keys);

    Map<int, String>::Element* current{identifier_to_scene_path.front()};
    while (current) {
        bool contained{false};
        int id{current->key()};
        const String& path{current->value()};
        for (int i = 0; i < keys.size(); ++i) {
            const Vector<int>& ids{group_to_identifiers[keys[i]]};
            if (ids.find(id) > -1) {
                contained = true;
                break;
            }
        }
        if (!contained) {
            removed_elements.push_back({id, path});
        } else {
            FileAccessRef file_access{FileAccess::create(FileAccess::ACCESS_RESOURCES)};
            if (!file_access->file_exists(path)) {
                removed_elements.push_back({id, path});
            }
        }
        current = current->next();
    }
    
    return removed_elements;
}

Map<int, String>::Element* PositionableSet::get_present_scenes_iterator() {
    return identifier_to_scene_path.front();
}

void PositionableSet::add_path_group(const String& p_path_group) {
    path_groups.append(p_path_group);
    group_to_identifiers[p_path_group] = Vector<int>();
}

void PositionableSet::add_positionable(int id, const String& path_group, const String& path) {
    group_to_identifiers[path_group].push_back(id);
    identifier_to_scene_path[id] = path;
}

void PositionableSet::remove_positionable(int id) {
    List<StringName> keys;
    group_to_identifiers.get_key_list(&keys);
    for (int i = 0; i < keys.size(); ++i) {
        Vector<int>& ids{group_to_identifiers[keys[i]]};
        int id_index{ids.find(id)};
        if (id_index > 0) {
            ids.remove(id_index);
            break;
        }
    }

    identifier_to_scene_path.erase(id);
}

bool PositionableSet::has_path_group(const String& p_path_group) const {
    for (int i = 0; i < path_groups.size(); ++i) {
        if (path_groups[i] == p_path_group) {
            return true;
        }
    }
    return false;
}

void PositionableSet::insert_positionable_if_not_present(const String& path_group, const String& resource_path) {
    int current_id{-1};
    Map<int, String>::Element* current{identifier_to_scene_path.front()};
    while (current) {
        if (current->value() == resource_path) {
            current_id = current->key();
        }
        if (current_id != -1) {
            break;
        } else {
            current = current->next();
        }
    }

    StringName path_group_hash{path_group};

    if (current_id == -1) {
        if (!group_to_identifiers.has(path_group_hash)) {
            group_to_identifiers[path_group_hash] = Vector<int>();
        }

        insert_positionable_for_path_group_and_id(path_group_hash, resource_path, ++last_id);
    } else {
        // if already contained but group path has changed, update group_to_identifier
        List<StringName> keys;
        group_to_identifiers.get_key_list(&keys);
        if (!group_to_identifiers.has(path_group_hash)) {
            group_to_identifiers[path_group_hash] = Vector<int>();
        }
        if (group_to_identifiers[path_group_hash].find(current_id) == -1) {
            group_to_identifiers[path_group_hash].push_back(current_id);
        }
    }
}


void PositionableSet::insert_positionable_for_path_group_and_id(const StringName& path_group, const String &resource_path,
                                                                int id) {
    Vector<int>& identifiers{group_to_identifiers[path_group]};
    identifiers.push_back(id);
    identifier_to_scene_path[id] = resource_path;
}

Dictionary PositionableSet::_get_group_to_identifiers() const {
    List<StringName> keys;
    group_to_identifiers.get_key_list(&keys);
    Dictionary converted;

    for (int i = 0; i < keys.size(); ++i) {
        StringName key{keys[i]};

        Array entry;
        const Vector<int>& identifiers{group_to_identifiers[key]};
        for (int j = 0; j < identifiers.size(); ++j) {
            entry.append(identifiers[j]);
        }
        converted[key.operator String()] = entry;
    }

    return converted;
}

void PositionableSet::_set_group_to_identifiers(const Dictionary& p_group_to_identifiers) {
    group_to_identifiers = HashMap<StringName, Vector<int>>();

    const Array& keys{p_group_to_identifiers.keys()};
    const Array& values{p_group_to_identifiers.values()};

    for (int i = 0; i < p_group_to_identifiers.size(); ++i) {
        Vector<int> entry;

        Array value{values[i]};
        for (int j = 0; j < value.size(); ++j) {
            entry.push_back(value[j]);
        }
        group_to_identifiers[keys[i]] = entry;
    }
}

int PositionableSet::_get_last_id() const {
    return last_id;
}

void PositionableSet::_set_last_id(int p_last_id) {
    last_id = p_last_id;
}

bool PositionableSet::path_groups_contains(const String& p_path_group) const {
    bool contained{false};

    for (int i = 0; i < path_groups.size(); ++i) {
        contained = path_groups[i] == p_path_group;
        if (contained) {
            break;
        }
    }
    
    return contained;
}

void PositionableSet::remove_not_anymore_presents_groups_to_identifiers() {
    List<StringName> keys;
    group_to_identifiers.get_key_list(&keys);
    for (int i = 0; i < keys.size(); ++i) {
        const StringName& path_group{keys[i]};
        if (!path_groups_contains(path_group)) {
            group_to_identifiers.erase(path_group);
        }
    }
}

#endif

Dictionary PositionableSet::_get_identifier_to_scene_path() const {
    Dictionary converted;
    
    Map<int, String>::Element* current{identifier_to_scene_path.front()};
    while (current) {
        converted[current->key()] = current->value();
        current = current->next();
    }
    
    return converted;
}

void PositionableSet::_set_identifier_to_scene_path(const Dictionary& p_identifier_to_scene_path) {
    identifier_to_scene_path = Map<int, String>();

    const Array& keys{p_identifier_to_scene_path.keys()};
    const Array& values{p_identifier_to_scene_path.values()};

    for (int i = 0; i < p_identifier_to_scene_path.size(); ++i) {
        int id{keys[i]};
        String value{values[i]};
        identifier_to_scene_path[id] = value;
    }
}

PositionableSet::PositionableSet() :
        Resource(),
        identifier_to_scene_path()
#ifdef TOOLS_ENABLED
        , path_groups(),
        group_to_identifiers(),
        last_id()
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

    ClassDB::bind_method(D_METHOD("_set_last_id", "p_last_id"), &PositionableSet::_set_last_id);
    ClassDB::bind_method(D_METHOD("_get_last_id"), &PositionableSet::_get_last_id);
    ADD_PROPERTY(
            PropertyInfo(Variant::INT, "last_id", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR | PROPERTY_USAGE_INTERNAL),
            "_set_last_id",
            "_get_last_id"
    );
    ADD_PROPERTY_DEFAULT("last_id", 0);
#endif

    ClassDB::bind_method(D_METHOD("_set_identifier_to_scene_path", "identifier_to_scene_path"),
                         &PositionableSet::_set_identifier_to_scene_path);
    ClassDB::bind_method(D_METHOD("_get_identifier_to_scene_path"), &PositionableSet::_get_identifier_to_scene_path);
    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "identifier_to_scene_path", PROPERTY_HINT_NONE, "",
                              PROPERTY_USAGE_NOEDITOR | PROPERTY_USAGE_INTERNAL), "_set_identifier_to_scene_path",
                 "_get_identifier_to_scene_path");
    ADD_PROPERTY_DEFAULT("identifier_to_scene_path", Dictionary());
}
