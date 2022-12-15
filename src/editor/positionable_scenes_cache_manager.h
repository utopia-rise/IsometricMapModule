#ifndef GODOT_POSITIONABLE_SCENES_CACHE_MANAGER_H
#define GODOT_POSITIONABLE_SCENES_CACHE_MANAGER_H

#ifdef TOOLS_ENABLED
    #include "node/isometric_positionable.h"

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

        void add_scene(Control* p_control, int index, Ref<PackedScene> scene);
        void clear();
        void clear(Control* p_control);
        Ref<Texture> get_icon(Control* p_control, int index);

        void copy_current_viewports_textures();
        void clear_current_viewports();

        void register_control(Control* p_control, const StringName& refresh_icon_function);
        void unregister_control(Control* p_control);
        void start_adding(Control* p_control, int cache_size);
        void end_adding(Control* p_control);
        bool is_adding() const;
        void refresh_all_icons() const;

        PositionableScenesCacheManager(const PositionableScenesCacheManager&) = delete;
        PositionableScenesCacheManager& operator=(const PositionableScenesCacheManager&) = delete;

    private:
        Map<Control*, Vector<CacheEntry>> cache;
        Map<Control*, Vector<Viewport*>> drawing_viewport;

        Map<Control*, bool> _is_adding;

        Map<Control*, StringName> refresh_icons_methods;

        static Viewport* _get_icon_for_scene(Ref<PackedScene> scene);

        PositionableScenesCacheManager();
        ~PositionableScenesCacheManager() = default;
    };
}// namespace editor

#endif
#endif// GODOT_POSITIONABLE_SCENES_CACHE_MANAGER_H
