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

        PositionableSet();
        ~PositionableSet() override = default;

    private:
        PoolStringArray positionable_path;
        HashMap<StringName, Vector<PackedScene>> scene_sets;
        void refresh_set();
        void insert_scene_if_positionable(const StringName &path);

    public:
        static void _bind_methods();
    };
}


#endif //ISOMETRIC_MAPS_POSITIONABLE_SET_H

#endif
