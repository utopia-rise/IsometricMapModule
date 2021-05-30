#ifdef TOOLS_ENABLED

#include <scene/2d/camera_2d.h>
#include <modules/isometric_maps/src/node/isometric_positionable.h>
#include <editor/editor_node.h>
#include <modules/isometric_maps/src/data/isometric_parameters.h>
#include <modules/isometric_maps/src/utils/isometric_maths.h>
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
        camera->add_child(positionable);
        viewport->set_update_mode(Viewport::UPDATE_ONCE);
        const utils::Hexagone& hexagone_coordinates{
            utils::get_hexagone_points(
                    data::IsometricParameters::getDefaultConfiguration(),
                    {positionable->get_local_position_3d(), positionable->get_size()}
            )
        };
        Vector2 scene_size{
            hexagone_coordinates.maxX - hexagone_coordinates.minX,
            hexagone_coordinates.maxY - hexagone_coordinates.minY
        };
        print_line(vformat("scene size is %s", scene_size));
        positionable->set_scale(positionable->get_scale() / scene_size);
        Vector2 viewport_size{128, 128};
        camera->set_position(viewport_size / 2);
        viewport->set_size(viewport_size);
        return viewport;
    }
    return nullptr;
}

PositionableScenesCacheManager::PositionableScenesCacheManager() : cache(), drawing_viewport(), _is_adding() {

}

#endif
