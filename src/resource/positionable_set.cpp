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
}

Vector<String> PositionableSet::get_scene_paths_for_group(const String& p_group) const {
    if (!group_to_identifiers.has(p_group)) {
        return {};
    }

    const Vector<int>& ids{group_to_identifiers[p_group]};

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
    remove_not_anymore_present_positionables();

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

    remove_not_anymore_present_positionables();

    emit_changed();
    return Error::OK;
}


void PositionableSet::remove_not_anymore_present_positionables() {
    Vector<int> ids_to_remove;
    Map<int, String>::Element* current{identifier_to_scene_path.front()};
    while (current) {
        const String& scene_path{current->value()};
        FileAccessRef file_access{FileAccess::create(FileAccess::ACCESS_RESOURCES)};
        if (!file_access->file_exists(scene_path)) {
            ids_to_remove.push_back(current->key());
        }
        current = current->next();
    }

    List<StringName> keys;
    group_to_identifiers.get_key_list(&keys);
    for (int i = 0; i < keys.size(); ++i) {
        bool contained{false};
        const StringName &key{keys[i]};
        for (int j = 0; j < path_groups.size(); ++j) {
            if (path_groups[j] == key) {
                contained = true;
                break;
            }
        }
        if (!contained) {
            ids_to_remove.append_array(group_to_identifiers[key]);
            group_to_identifiers.erase(key);
        }
    }

    for (int i = 0; i < ids_to_remove.size(); ++i) {
        int id{ids_to_remove[i]};
        bool contained;
        RemovedSetElement element_to_remove{id, identifier_to_scene_path[id]};
        for (int j = 0; j < removed_elements.size(); ++j) {
            contained = removed_elements[j].id == element_to_remove.id && removed_elements[j].element_path == element_to_remove.element_path;
            if (contained) break;
        }
        if (contained) continue;
        removed_elements.push_back(element_to_remove);
        identifier_to_scene_path.erase(id);
    }
}

const Vector<RemovedSetElement>& PositionableSet::get_removed_elements() const {
    return removed_elements;
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
            bool contained{false};
            Map<int, String>::Element* current{identifier_to_scene_path.front()};
            while (current) {
                contained = current->value() == resource_path;
                if (contained) {
                    break;
                } else {
                    current = current->next();
                }
            }
            
            if (!contained) {
                if (!group_to_identifiers.has(path_group)) {
                    group_to_identifiers[path_group] = Vector<int>();
                }

                for (int i = 0; i < removed_elements.size(); ++i) {
                    const RemovedSetElement& removed_element{removed_elements[i]};
                    if (removed_element.element_path == resource_path) {
                        insert_positionable_for_path_group_and_id(path_group, resource_path, removed_element.id);
                        removed_elements.remove(i);
                        memdelete(positionable);
                        return;
                    }
                }

                insert_positionable_for_path_group_and_id(path_group, resource_path, ++next_id);
            }
            memdelete(positionable);
        }
    }
}


void PositionableSet::insert_positionable_for_path_group_and_id(const String &path_group, const String &resource_path,
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
        identifier_to_scene_path()
#ifdef TOOLS_ENABLED
        , path_groups(),
        group_to_identifiers(),
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
    ClassDB::bind_method(D_METHOD("_get_identifier_to_scene_path"), &PositionableSet::_get_identifier_to_scene_path);
    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "identifier_to_scene_path", PROPERTY_HINT_NONE, "",
                              PROPERTY_USAGE_NOEDITOR | PROPERTY_USAGE_INTERNAL), "_set_identifier_to_scene_path",
                 "_get_identifier_to_scene_path");
    ADD_PROPERTY_DEFAULT("identifier_to_scene_path", Dictionary());
}
