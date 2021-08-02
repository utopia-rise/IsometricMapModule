#ifndef ISOMETRIC_MAPS_POSITIONABLE_SET_H
#define ISOMETRIC_MAPS_POSITIONABLE_SET_H

#include <core/resource.h>
#include <scene/resources/packed_scene.h>

namespace resource {

    class PositionableSet : public Resource {
        GDCLASS(PositionableSet, Resource)

#ifdef TOOLS_ENABLED
    public:
        struct RemovedElement {
            int id;
            String element_path;
        };
#endif

    public:
        void preload_scenes();
        bool is_set_loaded() const;
        String get_positionable_scene_path_for_id(int id) const;
        Ref<PackedScene> get_positionable_scene_for_id(int id) const;

#ifdef TOOLS_ENABLED
        const PoolStringArray& get_categories() const;
        void set_categories(const PoolStringArray& p_categories);

        Map<int, String> get_scene_paths_for_category(const String& p_group) const;

        bool add_category(const String& category);
        void remove_category(const String& category);
        void insert_positionable_if_not_present(const String& category, const String& resource_path);
        bool add_or_update_positionable(int id, const String& path);
        void remove_positionable(int id);
        bool has_category(const String& category) const;

        Map<int, String>::Element* get_present_scenes_iterator();

        Vector<resource::PositionableSet::RemovedElement> get_removed_elements() const;
#endif

        PositionableSet();
        ~PositionableSet() override = default;

    private:
        // exported, not visible in editor
        Map<int, String> identifier_to_scene_path;

        // Store loaded scenes
        Map<int, Ref<PackedScene>> identifier_to_loaded_scene;
        bool is_loaded;

        Dictionary _get_identifier_to_scene_path() const;
        void _set_identifier_to_scene_path(const Dictionary& p_identifier_to_scene_path);

#ifdef TOOLS_ENABLED
        // exported and visible in editor
        PoolStringArray categories;

        // exported, not visible in editor, here for set inspection purpose
        HashMap<StringName, Vector<int>> categories_to_identifiers;

        int last_id;

        Dictionary _get_categories_to_identifiers() const;
        void _set_categories_to_identifiers(const Dictionary& p_categories_to_identifiers);

        int _get_last_id() const;
        void _set_last_id(int p_last_id);

        void _insert_positionable_for_category_and_id(const StringName& category, const String& resource_path, int id);

#endif

    public:
        static void _bind_methods();
    };
}


#endif //ISOMETRIC_MAPS_POSITIONABLE_SET_H
