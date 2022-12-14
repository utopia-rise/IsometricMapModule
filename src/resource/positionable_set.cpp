#include "positionable_set.h"
#include <modules/isometric_maps/src/logging.h>
#include <modules/isometric_maps/src/node/isometric_positionable.h>

#ifdef TOOLS_ENABLED

    #include <core/io/resource_saver.h>
    #include <core/os/dir_access.h>
    #include <core/os/file_access.h>
    #include <modules/isometric_maps/src/editor/editor_utils.h>

#endif

using namespace resource;

void PositionableSet::preload_scenes() {
    identifier_to_loaded_scene.clear();

    Map<int, String>::Element* current {identifier_to_scene_path.front()};
    while (current) {
        _load_positionable_scene(current->key(), current->value());
        current = current->next();
    }

    if (!is_loaded) { is_loaded = true; }
}

bool PositionableSet::is_set_loaded() const {
    return is_loaded;
}

String PositionableSet::get_positionable_scene_path_for_id(int id) const {
    return identifier_to_scene_path[id];
}

Ref<PackedScene> PositionableSet::get_positionable_scene_for_id(int id) const {
    return identifier_to_loaded_scene[id];
}

#ifdef TOOLS_ENABLED

const PoolStringArray& PositionableSet::get_categories() const {
    return categories;
}

void PositionableSet::set_categories(const PoolStringArray& p_categories) {
    categories = p_categories;
}

Map<int, String> PositionableSet::get_scene_paths_for_category(const String& p_group) const {
    if (!categories_to_identifiers.has(p_group)) { return {}; }

    const Vector<int>& ids {categories_to_identifiers[p_group]};

    Map<int, String> paths_for_category;
    for (int i = 0; i < ids.size(); ++i) {
        const Variant& path {identifier_to_scene_path[ids[i]]};
        if (path.get_type() == Variant::STRING) { paths_for_category[ids[i]] = path; }
    }
    return paths_for_category;
}

Vector<resource::PositionableSet::RemovedElement> PositionableSet::get_removed_elements() const {
    Vector<RemovedElement> removed_elements;

    List<StringName> keys;
    categories_to_identifiers.get_key_list(&keys);

    Map<int, String>::Element* current {identifier_to_scene_path.front()};
    while (current) {
        int id {current->key()};
        const String& path {current->value()};
        FileAccessRef file_access {FileAccess::create(FileAccess::ACCESS_RESOURCES)};
        if (!file_access->file_exists(path)) { removed_elements.push_back({id, path}); }
        current = current->next();
    }

    return removed_elements;
}

Map<int, String>::Element* PositionableSet::get_present_scenes_iterator() {
    return identifier_to_scene_path.front();
}

bool PositionableSet::add_category(const String& category) {
    for (int i = 0; i < categories.size(); ++i) {
        if (categories[i] == category) { return false; }
    }

    categories.append(category);
    categories_to_identifiers[category] = Vector<int>();
    return true;
}

void PositionableSet::remove_category(const String& category) {
    const Vector<int>& category_ids {categories_to_identifiers[category]};

    for (int i = 0; i < category_ids.size(); ++i) {
        identifier_to_scene_path.erase(category_ids[i]);
    }

    categories_to_identifiers.erase(category);

    for (int i = 0; i < categories.size(); ++i) {
        if (categories[i] != category) continue;
        categories.remove(i);
        break;
    }
}

bool PositionableSet::add_or_update_positionable(int id, const String& path) {
    Map<int, String>::Element* current {identifier_to_scene_path.front()};
    while (current) {
        if (current->value() == path) { return false; }
        current = current->next();
    }
    identifier_to_scene_path[id] = path;

    if (is_loaded) { _load_positionable_scene(id, path); }

    return true;
}

void PositionableSet::remove_positionable(int id) {
    List<StringName> keys;
    categories_to_identifiers.get_key_list(&keys);
    for (int i = 0; i < keys.size(); ++i) {
        StringName& key {keys[i]};
        Vector<int>& ids {categories_to_identifiers[key]};
        int id_index = -1;
        for (int j = 0; j < ids.size(); ++j) {
            int current_id {ids[j]};
            if (current_id == id) {
                id_index = j;
                break;
            }
        }
        if (id_index >= 0) {
            ids.remove(id_index);
            break;
        }
    }

    identifier_to_scene_path.erase(id);
}

bool PositionableSet::has_category(const String& category) const {
    for (int i = 0; i < categories.size(); ++i) {
        if (categories[i] == category) { return true; }
    }
    return false;
}

void PositionableSet::insert_positionable_if_not_present(const String& category, const String& resource_path) {
    bool contained {false};
    Map<int, String>::Element* current {identifier_to_scene_path.front()};
    while (current) {
        if (current->value() == resource_path) { return; }
        current = current->next();
    }

    StringName category_hash {category};

    if (!categories_to_identifiers.has(category_hash)) { categories_to_identifiers[category_hash] = Vector<int>(); }

    _insert_positionable_for_category_and_id(category_hash, resource_path, ++last_id);
}

void PositionableSet::_insert_positionable_for_category_and_id(const StringName& category, const String& resource_path, int id) {
    Vector<int>& identifiers {categories_to_identifiers[category]};
    identifiers.push_back(id);
    identifier_to_scene_path[id] = resource_path;

    if (is_loaded) { _load_positionable_scene(id, resource_path); }
}

Dictionary PositionableSet::_get_categories_to_identifiers() const {
    List<StringName> keys;
    categories_to_identifiers.get_key_list(&keys);
    Dictionary converted;

    for (int i = 0; i < keys.size(); ++i) {
        StringName key {keys[i]};

        Array entry;
        const Vector<int>& identifiers {categories_to_identifiers[key]};
        for (int j = 0; j < identifiers.size(); ++j) {
            entry.append(identifiers[j]);
        }
        converted[key.operator String()] = entry;
    }

    return converted;
}

void PositionableSet::_set_categories_to_identifiers(const Dictionary& p_categories_to_identifiers) {
    categories_to_identifiers = HashMap<StringName, Vector<int>>();

    const Array& keys {p_categories_to_identifiers.keys()};
    const Array& values {p_categories_to_identifiers.values()};

    for (int i = 0; i < p_categories_to_identifiers.size(); ++i) {
        Vector<int> entry;

        Array value {values[i]};
        for (int j = 0; j < value.size(); ++j) {
            entry.push_back(value[j]);
        }
        categories_to_identifiers[keys[i]] = entry;
    }
}

int PositionableSet::_get_last_id() const {
    return last_id;
}

void PositionableSet::_set_last_id(int p_last_id) {
    last_id = p_last_id;
}

#endif

void PositionableSet::_load_positionable_scene(int id, const String& scene_path) {
    Ref<PackedScene> loaded {ResourceLoader::load(scene_path)};
    if (!loaded.is_valid()) { LOG_ERROR(vformat("Element %s from tileset is not valid.", scene_path)); }
    identifier_to_loaded_scene[id] = loaded;
}

Dictionary PositionableSet::_get_identifier_to_scene_path() const {
    Dictionary converted;

    Map<int, String>::Element* current {identifier_to_scene_path.front()};
    while (current) {
        converted[current->key()] = current->value();
        current = current->next();
    }

    return converted;
}

void PositionableSet::_set_identifier_to_scene_path(const Dictionary& p_identifier_to_scene_path) {
    identifier_to_scene_path = Map<int, String>();

    const Array& keys {p_identifier_to_scene_path.keys()};
    const Array& values {p_identifier_to_scene_path.values()};

    for (int i = 0; i < p_identifier_to_scene_path.size(); ++i) {
        int id {keys[i]};
        String value {values[i]};
        identifier_to_scene_path[id] = value;
    }
}

PositionableSet::PositionableSet() :
    Resource(),
    identifier_to_scene_path(),
    identifier_to_loaded_scene(),
    is_loaded(false)
#ifdef TOOLS_ENABLED
    ,
    categories(),
    categories_to_identifiers(),
    last_id()
#endif
{
}

void PositionableSet::_bind_methods() {
#ifdef TOOLS_ENABLED
    ClassDB::bind_method(D_METHOD("set_categories", "p_categories"), &PositionableSet::set_categories);
    ClassDB::bind_method(D_METHOD("get_categories"), &PositionableSet::get_categories);
    ADD_PROPERTY(
        PropertyInfo(Variant::ARRAY, "categories", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR | PROPERTY_USAGE_INTERNAL),
        "set_categories",
        "get_categories"
    );
    ADD_PROPERTY_DEFAULT("categories", PoolStringArray());

    ClassDB::bind_method(D_METHOD("_set_categories_to_identifiers", "p_categories_to_identifiers"), &PositionableSet::_set_categories_to_identifiers);
    ClassDB::bind_method(D_METHOD("_get_categories_to_identifiers"), &PositionableSet::_get_categories_to_identifiers);
    ADD_PROPERTY(
        PropertyInfo(Variant::DICTIONARY, "categories_to_identifiers", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR | PROPERTY_USAGE_INTERNAL),
        "_set_categories_to_identifiers",
        "_get_categories_to_identifiers"
    );
    ADD_PROPERTY_DEFAULT("categories_to_identifiers", Dictionary());

    ClassDB::bind_method(D_METHOD("_set_last_id", "p_last_id"), &PositionableSet::_set_last_id);
    ClassDB::bind_method(D_METHOD("_get_last_id"), &PositionableSet::_get_last_id);
    ADD_PROPERTY(
        PropertyInfo(Variant::INT, "last_id", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR | PROPERTY_USAGE_INTERNAL),
        "_set_last_id",
        "_get_last_id"
    );
    ADD_PROPERTY_DEFAULT("last_id", 0);
#endif

    ClassDB::bind_method(D_METHOD("_set_identifier_to_scene_path", "identifier_to_scene_path"), &PositionableSet::_set_identifier_to_scene_path);
    ClassDB::bind_method(D_METHOD("_get_identifier_to_scene_path"), &PositionableSet::_get_identifier_to_scene_path);
    ADD_PROPERTY(
        PropertyInfo(Variant::DICTIONARY, "identifier_to_scene_path", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR | PROPERTY_USAGE_INTERNAL),
        "_set_identifier_to_scene_path",
        "_get_identifier_to_scene_path"
    );
    ADD_PROPERTY_DEFAULT("identifier_to_scene_path", Dictionary());
}