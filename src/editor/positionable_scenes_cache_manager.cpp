#ifdef TOOLS_ENABLED

#include <scene/2d/camera_2d.h>
#include <modules/isometric_maps/src/node/isometric_positionable.h>
#include <editor/editor_node.h>
#include "positionable_scenes_cache_manager.h"
#include "isometric_editor_plugin.h"

using namespace editor;

PositionableScenesCacheManager& PositionableScenesCacheManager::get_instance() {
    static PositionableScenesCacheManager instance;
    return instance;
}

Ref<PackedScene> PositionableScenesCacheManager::get_scene(int index) {
    return cache[index].scene;
}

void PositionableScenesCacheManager::add_scene(int index, Ref<PackedScene> scene) {
    Viewport* scene_viewport{_get_icon_for_scene(scene)};
    if (scene_viewport) {
        EditorNode::get_singleton()->add_child(scene_viewport);
        drawing_viewport.set(index, scene_viewport);
        cache.set(index, {scene, scene_viewport->get_texture()});
    }
}

void PositionableScenesCacheManager::clear() {
    cache.clear();
}

Ref<Texture> PositionableScenesCacheManager::get_icon(int index) {
    return cache[index].icon;
}

void PositionableScenesCacheManager::copy_current_viewports_textures() {
    for (int i = 0; i < drawing_viewport.size(); ++i) {
        Ref<ImageTexture> copy_texture;
        copy_texture.instance();
        copy_texture->create_from_image(drawing_viewport[i]->get_texture()->get_data());
        CacheEntry copy{cache.get(i)};
        copy.icon = copy_texture;
        cache.set(i, copy);
    }
}

void PositionableScenesCacheManager::clear_current_viewports() {
    for (int i = 0; i < drawing_viewport.size(); ++i) {
        Viewport* viewport{drawing_viewport[i]};
        EditorNode::get_singleton()->remove_child(viewport);
        viewport->queue_delete();
    }
    drawing_viewport.clear();
}

void PositionableScenesCacheManager::start_adding(int cache_size) {
    _is_adding = true;
    drawing_viewport.resize(cache_size);
    cache.resize(cache_size);
}

void PositionableScenesCacheManager::end_adding() {
    _is_adding = false;
    IsometricEditorPlugin::get_instance()->set_should_clear_buffer_on_next_frame(false);
}

bool PositionableScenesCacheManager::is_adding() const {
    return _is_adding;
}

Viewport* PositionableScenesCacheManager::_get_icon_for_scene(Ref<PackedScene> scene) {
    if (auto* positionable{Object::cast_to<node::IsometricPositionable>(scene->instance())}) {
        const Vector2& original_scale{positionable->get_scale()};
        positionable->set_scale(original_scale * Vector2(1, -1));

        Viewport* viewport{memnew(Viewport)};
        Camera2D* camera{memnew(Camera2D)};
        viewport->add_child(camera);
        viewport->add_child(positionable);
        viewport->set_update_mode(Viewport::UPDATE_ONCE);
//        Vector2 size{
//            static_cast<float>(IsometricServer::get_instance()->get_isometric_space_diamond_width(positionable->get_space_RID())),
//            static_cast<float>(IsometricServer::get_instance()->get_isometric_space_diamond_height(positionable->get_space_RID()))
//        };
        viewport->set_size({100, 100});
        return viewport;
    }
    return nullptr;
}

PositionableScenesCacheManager::PositionableScenesCacheManager() : cache(), drawing_viewport(), _is_adding() {

}

#endif
