#ifdef TOOLS_ENABLED

#ifndef GODOT_POSITIONABLE_SCENES_CACHE_MANAGER_H
#define GODOT_POSITIONABLE_SCENES_CACHE_MANAGER_H

#include <core/hash_map.h>
#include <core/reference.h>
#include <scene/resources/packed_scene.h>

namespace editor {
    class PositionableScenesCacheManager {
    public:
        static PositionableScenesCacheManager& get_instance();

        Ref<PackedScene>& get_scene(int index);
        void add_scene(int index, Ref<PackedScene> scene);
        void clear();

        PositionableScenesCacheManager(const PositionableScenesCacheManager&) = delete;
        PositionableScenesCacheManager& operator=(const PositionableScenesCacheManager&) = delete;

    private:
        HashMap<int, Ref<PackedScene>> scenes;

        PositionableScenesCacheManager();
        ~PositionableScenesCacheManager() = default;
    };
}


#endif //GODOT_POSITIONABLE_SCENES_CACHE_MANAGER_H

#endif
