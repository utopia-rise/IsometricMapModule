#ifndef ISOMETRIC_MAPS_POSITIONABLE_SET_H
#define ISOMETRIC_MAPS_POSITIONABLE_SET_H

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
        Ref<PackedScene> get_positionable_scene_for_id(int id) const;

        static constexpr int NONE_POSITIONABLE_ID = -1;

#ifdef TOOLS_ENABLED
        const PackedStringArray& get_categories() const;
        void set_categories(const PackedStringArray& p_categories);

        HashMap<int, String> get_scene_paths_for_category(const String& p_group) const;

        bool add_category(const String& category);
        void remove_category(const String& category);
        void insert_positionable_if_not_present(const String& category, const String& resource_path);
        bool add_or_update_positionable(int id, const String& path);
        void remove_positionable(int id);

        Vector<resource::PositionableSet::RemovedElement> get_removed_elements() const;

#endif

        PositionableSet();
        ~PositionableSet() override = default;

    private:
        // exported, not is_visible in editor
        HashMap<int, String> identifier_to_scene_path;

        // Store loaded scenes
        HashMap<int, Ref<PackedScene>> identifier_to_loaded_scene;
        bool is_loaded;

        void _load_positionable_scene(int id, const String& scene_path);
        Dictionary _get_identifier_to_scene_path() const;
        void _set_identifier_to_scene_path(const Dictionary& p_identifier_to_scene_path);

#ifdef TOOLS_ENABLED
        // exported and is_visible in editor
        PackedStringArray categories;

        // exported, not is_visible in editor, here for set inspection purpose
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
}// namespace resource

#endif// ISOMETRIC_MAPS_POSITIONABLE_SET_H
