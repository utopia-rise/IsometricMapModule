#ifdef TOOLS_ENABLED

#include <modules/isometric_maps/src/node/isometric_positionable.h>
#include <scene/main/viewport.h>
#include <scene/2d/camera_2d.h>
#include <editor/editor_node.h>
#include <modules/isometric_maps/src/editor/isometric_editor_plugin.h>
#include <modules/isometric_maps/src/editor/positionable_scenes_cache_manager.h>
#include "positionable_selection_pane.h"

using namespace editor::inspector;

void PositionableSelectionPane::set_positionable_set(const Ref<resource::PositionableSet>& set) {
    positionable_set = set;
    if (positionable_set.is_valid() &&
            !positionable_set->is_connected("changed", this, "_refresh_path_selector")) {
        positionable_set->connect("changed", this, "_refresh_path_selector");
    }
    _refresh_path_selector();
}

void PositionableSelectionPane::_refresh_path_selector() {
    path_selector->clear();
    if (positionable_set.is_valid()) {
        const PoolStringArray &paths{positionable_set->get_positionable_paths()};
        for (int i = 0; i < paths.size(); ++i) {
            path_selector->add_item(paths[i]);
        }
    }
    int selected_index{path_selector->get_selected()};
    if (selected_index >= 0) {
        _select_item_from_path_selector(selected_index);
    }
}

void PositionableSelectionPane::_notification(int notif) {
    if (notif == NOTIFICATION_READY) {
        _ready();
    }
}

void PositionableSelectionPane::_ready() {
    path_selector->connect("item_selected", this, "_select_item_from_path_selector");
}

void PositionableSelectionPane::_select_item_from_path_selector(int index) {
    print_line(vformat("selected path %s", index));
    const Vector<Ref<PackedScene>>& scenes{
        positionable_set->get_storage_for_path(path_selector->get_item_text(index))
    };
    PositionableScenesCacheManager::get_instance().clear();
    positionable_inspector->clear();
    for (int i = 0; i < scenes.size(); ++i) {
        const Ref<PackedScene>& positionable_scene{scenes[i]};
        StringName path{positionable_scene->get_path()};

        Viewport* icon_viewport{_get_icon_for_scene(positionable_scene)};
        IsometricEditorPlugin::get_instance()->add_icon_viewport(icon_viewport);
        Ref<ViewportTexture> icon_texture{icon_viewport->get_texture()};
        positionable_inspector->add_item(positionable_scene->get_path(), icon_texture);
        PositionableScenesCacheManager::get_instance().add_scene(positionable_inspector->get_item_count(), positionable_scene);
    }
}

PositionableSelectionPane::PositionableSelectionPane() : VSplitContainer(), path_selector(memnew(OptionButton)),
                                                         positionable_set(), positionable_inspector(memnew(ItemList)) {
    add_child(path_selector);
    add_child(positionable_inspector);
    _refresh_path_selector();
}

void PositionableSelectionPane::_bind_methods() {
    ClassDB::bind_method("_refresh_path_selector", &PositionableSelectionPane::_refresh_path_selector);
    ClassDB::bind_method("_select_item_from_path_selector", &PositionableSelectionPane::_select_item_from_path_selector);
    ClassDB::bind_method("set_positionable_set", &PositionableSelectionPane::set_positionable_set);
}

Viewport *PositionableSelectionPane::_get_icon_for_scene(Ref<PackedScene> scene) {
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
        EditorNode::get_singleton()->add_child(viewport);
        return viewport;
    }
    return nullptr;
}

#endif
