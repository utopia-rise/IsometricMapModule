#ifdef TOOLS_ENABLED

#ifndef GODOT_POSITIONABLE_SCENES_CACHE_MANAGER_H
#define GODOT_POSITIONABLE_SCENES_CACHE_MANAGER_H

#include <core/hash_map.h>
#include <core/reference.h>
#include <scene/resources/packed_scene.h>

namespace editor {
    class PositionableScenesCacheManager {
        struct CacheEntry {
            Ref<PackedScene> scene;
            Ref<Texture> icon;
        };

    public:
        static PositionableScenesCacheManager& get_instance();

        Ref<PackedScene>& get_scene(int index);
        void add_scene(int index, Ref<PackedScene> scene);
        void clear();
        Ref<Texture> get_icon(int index);

        void copy_current_viewports_textures();
        void clear_current_viewports();

        void start_adding();
        void end_adding();
        bool is_adding() const;

        PositionableScenesCacheManager(const PositionableScenesCacheManager&) = delete;
        PositionableScenesCacheManager& operator=(const PositionableScenesCacheManager&) = delete;

    private:
        HashMap<int, CacheEntry> cache;
        HashMap<int, Viewport*> drawing_viewport;

        bool _is_adding;

        static Viewport* _get_icon_for_scene(Ref<PackedScene> scene);

        PositionableScenesCacheManager();
        ~PositionableScenesCacheManager() = default;
    };
}


#endif //GODOT_POSITIONABLE_SCENES_CACHE_MANAGER_H

#endif
