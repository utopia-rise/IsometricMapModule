#ifndef ISOMETRIC_MAPS_POSITIONABLE_SET_H
#define ISOMETRIC_MAPS_POSITIONABLE_SET_H

#include <core/resource.h>
#include <scene/resources/packed_scene.h>

namespace resource {
    class PositionableSet : public Resource {
        GDCLASS(PositionableSet, Resource)

    public:
        const PoolStringArray& get_positionable_paths() const;
        void set_positionable_paths(const PoolStringArray& paths);
        const Vector<Ref<PackedScene>>& get_storage_for_path(const StringName &path);
        Error refresh_set();

        PositionableSet();
        ~PositionableSet() override = default;

    private:
        PoolStringArray positionable_paths;
        HashMap<StringName, Vector<Ref<PackedScene>>> scenes_storage_map;
        Error insert_all_positionables_for_path(const String &path, const char *current_hash);
        void insert_scene_if_positionable(const StringName &hash, const String &path);

    public:
        static void _bind_methods();
    };
}


#endif //ISOMETRIC_MAPS_POSITIONABLE_SET_H
