#ifdef TOOLS_ENABLED

#include <scene/2d/camera_2d.h>
#include <modules/isometric_maps/src/node/isometric_positionable.h>
#include <editor/editor_node.h>
#include <modules/isometric_maps/src/data/isometric_parameters.h>
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
        const Transform2D& hexagone_coordinates{_get_hexagone_coordinates(positionable)};
        Vector2 scene_size{
            hexagone_coordinates.get_axis(0).y - hexagone_coordinates.get_axis(0).x,
            hexagone_coordinates.get_axis(1).y - hexagone_coordinates.get_axis(1).x
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

Transform2D PositionableScenesCacheManager::_get_hexagone_coordinates(node::IsometricPositionable *positionable) {
    const Vector3& position_3d{positionable->get_local_position_3d()};
    const Vector3& size_3d{positionable->get_size()};

    Vector3 upper_point{
        position_3d.x,
        position_3d.y,
        position_3d.z + data::IsometricParameters::getDefaultConfiguration().z_ratio * size_3d.z
    };
    Vector3 lower_point{
        position_3d.x + size_3d.x,
        position_3d.y + size_3d.y,
        position_3d.z
    };
    Vector3 left_point{
        position_3d.x,
        position_3d.y + size_3d.y,
        position_3d.z
    };
    Vector3 right_point{
        position_3d.x + size_3d.x,
        position_3d.y,
        position_3d.z
    };
    return {
        upper_point.x - upper_point.z,
        lower_point.x - lower_point.z,
        upper_point.y - upper_point.z,
        lower_point.y - lower_point.z,
        left_point.x - left_point.y,
        right_point.x - right_point.y
    };
}

PositionableScenesCacheManager::PositionableScenesCacheManager() : cache(), drawing_viewport(), _is_adding() {

}

#endif
