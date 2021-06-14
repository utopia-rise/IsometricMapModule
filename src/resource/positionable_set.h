#ifndef ISOMETRIC_MAPS_POSITIONABLE_SET_H
#define ISOMETRIC_MAPS_POSITIONABLE_SET_H

#include <core/resource.h>
#include <scene/resources/packed_scene.h>

namespace resource {

#ifdef TOOLS_ENABLED
    struct RemovedSetElement {
        int id;
        String element_path;
    };
#endif

    class PositionableSet : public Resource {
        GDCLASS(PositionableSet, Resource)

    public:
        void get_positionable_scene_path_for_id(int id, String& r_path) const;

#ifdef TOOLS_ENABLED
        const PoolStringArray& get_path_groups() const;
        void set_path_groups(const PoolStringArray& paths);

        Vector<String> get_scene_paths_for_group(const String& p_group) const;
        Error refresh_set();

        const Vector<RemovedSetElement>& get_removed_elements() const;
        void insert_path_at_id(const String& existing_group_path, int id, const String& scene_path);
#endif

        PositionableSet();
        ~PositionableSet() override = default;

    private:
        // exported, not visible in editor
        Dictionary identifier_to_scene_path;

        const Dictionary& _get_identifier_to_scene_path() const;
        void _set_identifier_to_scene_path(const Dictionary& p_identifier_to_scene_path);

#ifdef TOOLS_ENABLED
        enum EditorCheckSetterCall {
            PATH_GROUPS,
            GROUP_TO_IDENTIFIER,
            IDENTIFIER_TO_SCENE_PATH
        };

        // exported and visible in editor
        PoolStringArray path_groups;

        // exported, not visible in editor, here for set inspection purpose
        Dictionary group_to_identifiers;

        int editor_check_set_call;

        int next_id;

        Vector<RemovedSetElement> removed_elements;

        const Dictionary& _get_group_to_identifiers() const;
        void _set_group_to_identifiers(const Dictionary& p_group_to_identifiers);

        inline bool is_data_set() const;
        Error insert_all_positionables_for_path_if_not_present(const String& path, const char* base_path);
        void insert_positionable_scene_if_not_present(const String& path_group, const String& resource_path);


#endif

    public:
        static void _bind_methods();
    };
}


#endif //ISOMETRIC_MAPS_POSITIONABLE_SET_H
