#ifdef TOOLS_ENABLED

#include <modules/isometric_maps/src/node/isometric_positionable.h>
#include <scene/main/viewport.h>
#include <scene/2d/camera_2d.h>
#include <editor/editor_node.h>
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

int PositionableSelectionPane::get_selected_positionable_index() const {
    const Vector<int>& selected_items{item_list->get_selected_items()};
    if (selected_items.empty()) {
        return -1;
    }
    return selected_items[0];
}

void PositionableSelectionPane::refresh_icons() {
    for (int i = 0; i < item_list->get_item_count(); ++i) {
        item_list->set_item_icon(i, PositionableScenesCacheManager::get_instance().get_icon(i));
    }
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
    } else {
        item_list->clear();
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
    const Vector<Ref<PackedScene>>& scenes{
        positionable_set->get_storage_for_path(path_selector->get_item_text(index))
    };
    PositionableScenesCacheManager::get_instance().clear();
    item_list->clear();
    PositionableScenesCacheManager::get_instance().start_adding();
    for (int i = 0; i < scenes.size(); ++i) {
        const Ref<PackedScene>& positionable_scene{scenes[i]};
        StringName path{positionable_scene->get_path()};

        PositionableScenesCacheManager::get_instance().add_scene(item_list->get_item_count(), positionable_scene);
        Ref<Texture> icon_texture{PositionableScenesCacheManager::get_instance().get_icon(item_list->get_item_count())};
        item_list->add_item(positionable_scene->get_path(), icon_texture);
    }
    PositionableScenesCacheManager::get_instance().end_adding();
}

void PositionableSelectionPane::_refresh_current_set() {
    if (positionable_set.is_valid()) {
        positionable_set->refresh_set();
    }
}

PositionableSelectionPane::PositionableSelectionPane() : VSplitContainer(), path_selector(memnew(OptionButton)),
                                                         item_list(memnew(ItemList)), positionable_set() {
    add_child(path_selector);
    add_child(item_list);
    _refresh_path_selector();
}

void PositionableSelectionPane::_bind_methods() {
    ClassDB::bind_method("_refresh_path_selector", &PositionableSelectionPane::_refresh_path_selector);
    ClassDB::bind_method("_refresh_current_set", &PositionableSelectionPane::_refresh_current_set);
    ClassDB::bind_method("_select_item_from_path_selector", &PositionableSelectionPane::_select_item_from_path_selector);
    ClassDB::bind_method("set_positionable_set", &PositionableSelectionPane::set_positionable_set);
}

#endif
