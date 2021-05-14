#ifdef TOOLS_ENABLED

#include "positionable_scenes_cache_manager.h"

using namespace editor;

PositionableScenesCacheManager& PositionableScenesCacheManager::get_instance() {
    static PositionableScenesCacheManager instance;
    return instance;
}

Ref<PackedScene>& PositionableScenesCacheManager::get_scene(int index) {
    return scenes[index];
}

void PositionableScenesCacheManager::add_scene(int index, Ref<PackedScene> scene) {
    scenes[index] = scene;
}

void PositionableScenesCacheManager::clear() {
    scenes.clear();
}

PositionableScenesCacheManager::PositionableScenesCacheManager() : scenes() {

}

#endif
