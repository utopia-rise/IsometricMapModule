#ifdef TOOLS_ENABLED

#ifndef ISOMETRIC_MAPS_POSITIONABLE_SET_H
#define ISOMETRIC_MAPS_POSITIONABLE_SET_H

#include <core/resource.h>
#include <scene/resources/packed_scene.h>

namespace editor {
    class PositionableSet : public Resource {
        GDCLASS(PositionableSet, Resource)

    public:
        const PoolStringArray& get_positionable_paths() const;
        void set_positionable_paths(const PoolStringArray& paths);

        struct PositionableSceneStorage {
            Vector<Ref<PackedScene>> maps;
            Vector<Ref<PackedScene>> positionables;
        };

        PositionableSet();
        ~PositionableSet() override = default;

    private:
        PoolStringArray positionable_paths;
        HashMap<StringName, PositionableSceneStorage> scene_sets;
        void refresh_set();
        void insert_all_positionables_for_path(const String& path);
        void insert_scene_if_positionable(const StringName& path);

    public:
        static void _bind_methods();
    };
}


#endif //ISOMETRIC_MAPS_POSITIONABLE_SET_H

#endif
