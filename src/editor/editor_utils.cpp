#ifdef TOOLS_ENABLED

    #include "editor_utils.h"
    #include "positionable_scenes_cache_manager.h"
    #include "positionable_set_editor_plugin.h"
    #include <core/os/dir_access.h>
    #include <core/os/file_access.h>
    #include <core/resource.h>
    #include <core/variant.h>
    #include <modules/isometric_maps/src/logging.h>
    #include <modules/isometric_maps/src/node/isometric_positionable.h>
    #include <modules/isometric_maps/src/resource/positionable_set.h>
    #include <scene/resources/packed_scene.h>

using namespace editor;

Error EditorUtils::find_all_positionables_in_path(const String& path, List<String>* r_value) {
    Error error;
    DirAccess* dir_access {DirAccess::open(path, &error)};

    if (error != OK) {
        FileAccess* file_access {FileAccess::open(path, FileAccess::READ, &error)};
        if (error != OK) {
            LOG_WARNING(vformat("%s cannot be opened", path));
            return Error::ERR_CANT_RESOLVE;
        }
        if (path.ends_with(".tscn")) {
            RES resource {ResourceLoader::load(path)};
            if (auto* packed_scene {Object::cast_to<PackedScene>(resource.ptr())}) {
                // TODO: investigate get_inheritance_list_static generated from GDClass
                if (auto* positionable {Object::cast_to<node::IsometricPositionable>(packed_scene->instance())}) {
                    r_value->push_back(path);
                    memdelete(positionable);
                }
            }
        }
        file_access->close();
        memdelete(file_access);
    } else {
        dir_access->list_dir_begin();
        String item;
        while (!(item = dir_access->get_next()).empty()) {
            if (item == "." || item == "..") { continue; }
            find_all_positionables_in_path(path.plus_file(item), r_value);
        }
        memdelete(dir_access);
    }
    return Error::OK;
}

void EditorUtils::refresh_item_list_containing_tiles_for_category(
    const String& category, ItemList* item_list, const Ref<resource::PositionableSet>& positionable_set, Control* calling_control
) {
    const Map<int, String>& scene_paths {positionable_set->get_scene_paths_for_category(category)};

    Vector<Ref<PackedScene>> scenes_to_add;
    Vector<Ref<PositionableItemListMetadata>> scenes_to_add_metadata;
    Map<int, String>::Element* current {scene_paths.front()};
    while (current) {
        String& positionable_path = current->value();
        FileAccessRef file_access {FileAccess::create(FileAccess::ACCESS_RESOURCES)};
        if (file_access->file_exists(positionable_path)) {
            scenes_to_add.push_back(ResourceLoader::load(positionable_path));
            Ref<PositionableItemListMetadata> metadata;
            metadata.instance();
            metadata->positionable_id = current->key();
            metadata->is_valid = true;
            scenes_to_add_metadata.push_back(metadata);
        }
        current = current->next();
    }

    int scene_to_add_size {scenes_to_add.size()};
    PositionableScenesCacheManager::get_instance().start_adding(calling_control, scene_to_add_size);

    for (int i = 0; i < scene_to_add_size; ++i) {
        const Ref<PackedScene>& positionable_scene {scenes_to_add[i]};
        const String& path {positionable_scene->get_path()};

        PositionableScenesCacheManager::get_instance().add_scene(calling_control, item_list->get_item_count(), positionable_scene);
        Ref<Texture> icon_texture {PositionableScenesCacheManager::get_instance().get_icon(calling_control, item_list->get_item_count())};
        item_list->add_item(path, icon_texture);
        item_list->set_item_metadata(item_list->get_item_count() - 1, scenes_to_add_metadata[i]);
    }

    PositionableScenesCacheManager::get_instance().end_adding(calling_control);

    const Vector<resource::PositionableSet::RemovedElement>& removed_elements {positionable_set->get_removed_elements()};

    for (int i = 0; i < removed_elements.size(); ++i) {
        const resource::PositionableSet::RemovedElement& element {removed_elements[i]};
        item_list->add_item(element.element_path);
        int last_item_id {item_list->get_item_count() - 1};
        Ref<PositionableItemListMetadata> metadata;
        metadata.instance();
        metadata->positionable_id = element.id;
        metadata->is_valid = false;
        item_list->set_item_metadata(last_item_id, metadata);
        item_list->set_item_custom_bg_color(last_item_id, Color(0.65, 0.14, 0));
        item_list->set_item_icon(last_item_id, EditorInterface::get_singleton()->get_base_control()->get_icon("Remove", "EditorIcons"));
    }
}

bool EditorUtils::item_from_list_has_metadata(ItemList* item_list, int index) {
    return item_list->get_item_metadata(index).get_type() != Variant::NIL;
}

bool EditorUtils::item_from_list_has_valid_tile_metadata(ItemList* item_list, int index) {
    if (!item_from_list_has_metadata(item_list, index)) return false;
    if (auto* metadata {Object::cast_to<PositionableItemListMetadata>(item_list->get_item_metadata(index))}) {
        if (!metadata->is_valid) return false;
        return true;
    }
    return false;
}

void EditorUtils::refresh_positionable_icons_for_item_list(ItemList* item_list, Control* registered_in_manager) {
    for (int i = 0; i < item_list->get_item_count(); ++i) {
        if (!item_from_list_has_valid_tile_metadata(item_list, i)) continue;
        item_list->set_item_icon(i, PositionableScenesCacheManager::get_instance().get_icon(registered_in_manager, i));
    }
}

#endif
