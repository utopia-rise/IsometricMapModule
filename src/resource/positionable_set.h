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
        void get_positionable_scene_path_for_id(int id, String& r_path) const;

#ifdef TOOLS_ENABLED
        const PoolStringArray& get_path_groups() const;
        void set_path_groups(const PoolStringArray& paths);

        Map<int, String> get_scene_paths_for_group(const String& p_group) const;
        Error refresh_set();

        void add_path_group(const String& p_path_group);
        void add_positionable(int id, const String& path_group, const String& path);
        void remove_positionable(int id);
        bool has_path_group(const String& p_path_group) const;

        Map<int, String>::Element* get_present_scenes_iterator();

        Vector<resource::PositionableSet::RemovedElement> get_removed_elements() const;
#endif

        PositionableSet();
        ~PositionableSet() override = default;

    private:
        // exported, not visible in editor
        Map<int, String> identifier_to_scene_path;

        Dictionary _get_identifier_to_scene_path() const;
        void _set_identifier_to_scene_path(const Dictionary& p_identifier_to_scene_path);

#ifdef TOOLS_ENABLED
        // exported and visible in editor
        PoolStringArray path_groups;

        // exported, not visible in editor, here for set inspection purpose
        HashMap<StringName, Vector<int>> group_to_identifiers;

        int last_id;

        Dictionary _get_group_to_identifiers() const;
        void _set_group_to_identifiers(const Dictionary& p_group_to_identifiers);

        int _get_last_id() const;
        void _set_last_id(int p_last_id);

        void insert_positionable_if_not_present(const String& path_group, const String& resource_path);
        void insert_positionable_for_path_group_and_id(const StringName& path_group, const String& resource_path, int id);

        bool path_groups_contains(const String& p_path_group) const;
        void remove_not_anymore_presents_groups_to_identifiers();

#endif

    public:
        static void _bind_methods();
    };
}


#endif //ISOMETRIC_MAPS_POSITIONABLE_SET_H
