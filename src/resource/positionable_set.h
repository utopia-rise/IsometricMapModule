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
        Error refresh_set();

        struct PositionableSceneStorage {
            Vector<Ref<PackedScene>> maps;
            Vector<Ref<PackedScene>> positionables;
        };

        PositionableSet();
        ~PositionableSet() override = default;

    private:
        PoolStringArray positionable_paths;
        HashMap<StringName, PositionableSceneStorage> scene_sets;
        Error insert_all_positionables_for_path(const String& path);
        void insert_scene_if_positionable(const StringName& path);

    public:
        static void _bind_methods();
    };
}


#endif //ISOMETRIC_MAPS_POSITIONABLE_SET_H
