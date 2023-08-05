#ifdef TOOLS_ENABLED
#include "positionable_scenes_cache_manager.h"

#include "data/isometric_parameters.h"
#include "isometric_editor_plugin.h"
#include "utils/isometric_maths.h"

#include <scene/2d/camera_2d.h>

using namespace editor;

PositionableScenesCacheManager& PositionableScenesCacheManager::get_instance() {
    static PositionableScenesCacheManager instance;
    return instance;
}

void PositionableScenesCacheManager::add_scene(Control* p_control, int index, Ref<PackedScene> scene) {
    SubViewport* scene_viewport {_get_icon_for_scene(scene)};
    if (scene_viewport) {
        EditorNode::get_singleton()->add_child(scene_viewport);
        if (!drawing_viewport.has(p_control)) {
            return;
        }
        drawing_viewport[p_control].set(index, scene_viewport);
        cache[p_control].set(index, {scene, scene_viewport->get_texture()});
    }
}

void PositionableScenesCacheManager::clear() {
    cache.clear();
}

void PositionableScenesCacheManager::clear(Control* p_control) {
    cache[p_control].clear();
}

Ref<Texture> PositionableScenesCacheManager::get_icon(Control* p_control, int index) {
    return cache[p_control][index].icon;
}

void PositionableScenesCacheManager::copy_current_viewports_textures() {
    for (KeyValue<Control*, Vector<SubViewport*>> entry : drawing_viewport) {
        Control* control {entry.key};
        const Vector<SubViewport*>& viewports {entry.value};
        for (int i = 0; i < viewports.size(); ++i) {
            CacheEntry copy {cache[control].get(i)};
            Ref<Image> texture_image {viewports[i]->get_texture()->get_image()};
            texture_image->flip_y();
            copy.icon = ImageTexture::create_from_image(texture_image);
            cache[control].set(i, copy);
        }
    }
}

void PositionableScenesCacheManager::clear_current_viewports() {
    for (KeyValue<Control*, Vector<SubViewport*>>& entry : drawing_viewport) {
        Vector<SubViewport*>& viewports {entry.value};
        for (int i = 0; i < viewports.size(); ++i) {
            Viewport* viewport {viewports[i]};
            EditorNode::get_singleton()->remove_child(viewport);
            memdelete(viewport);
        }
        viewports.clear();
    }
}

void PositionableScenesCacheManager::register_control(Control* p_control, const StringName& refresh_icon_function) {
    cache[p_control] = Vector<CacheEntry>();
    drawing_viewport[p_control] = Vector<SubViewport*>();
    _is_adding[p_control] = false;
    refresh_icons_methods[p_control] = refresh_icon_function;
}

void PositionableScenesCacheManager::unregister_control(Control* p_control) {
    cache.erase(p_control);
    for (SubViewport* viewport : drawing_viewport[p_control]) {
        EditorNode::get_singleton()->remove_child(viewport);
        memdelete(viewport);
    }
    drawing_viewport.erase(p_control);
    _is_adding.erase(p_control);
    refresh_icons_methods.erase(p_control);
}

void PositionableScenesCacheManager::start_adding(Control* p_control, int cache_size) {
    _is_adding[p_control] = true;
    drawing_viewport[p_control].resize(cache_size);
    cache[p_control].resize(cache_size);
}

void PositionableScenesCacheManager::end_adding(Control* p_control) {
    _is_adding[p_control] = false;
    IsometricEditorPlugin::get_instance()->set_should_clear_buffer_on_next_frame(is_adding());
}

bool PositionableScenesCacheManager::is_adding() const {
    for (KeyValue<Control*, bool> entry : _is_adding) {
        if (entry.value) { return true; }
    }
    return false;
}

void PositionableScenesCacheManager::refresh_all_icons() const {
    for (KeyValue<Control*, StringName> entry : refresh_icons_methods) {
        entry.key->call(entry.value);
    }
}

SubViewport* PositionableScenesCacheManager::_get_icon_for_scene(Ref<PackedScene> scene) {
    if (auto* positionable {Object::cast_to<node::IsometricPositionable>(scene->instantiate())}) {
        const Vector2& original_scale {positionable->get_scale()};
        positionable->set_scale(original_scale * Vector2(1, -1));

        SubViewport* viewport {memnew(SubViewport)};
        Camera2D* camera {memnew(Camera2D)};
        viewport->add_child(camera);
        camera->add_child(positionable);
        viewport->set_update_mode(SubViewport::UPDATE_ONCE);
        const utils::Hexagone& hexagone_coordinates {utils::get_hexagone_points(
          data::IsometricParameters::getDefaultConfiguration(),
          {positionable->get_local_position_3d(), positionable->get_size()}
        )};
        Vector2 scene_size {
          hexagone_coordinates.maxX - hexagone_coordinates.minX,
          hexagone_coordinates.maxY - hexagone_coordinates.minY};
        positionable->set_scale(positionable->get_scale() / scene_size);
        Vector2 viewport_size {128, 128};
        camera->set_position(viewport_size / 2);
        viewport->set_size(viewport_size);
        return viewport;
    }
    return nullptr;
}

void PositionableScenesCacheManager::shutdown() {
    clear_current_viewports();
    cache.clear();
    _is_adding.clear();
    refresh_icons_methods.clear();
}

PositionableScenesCacheManager::PositionableScenesCacheManager() {}

#endif
