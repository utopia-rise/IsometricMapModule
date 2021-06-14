#ifdef TOOLS_ENABLED

#include <scene/main/viewport.h>
#include <editor/editor_node.h>
#include <modules/isometric_maps/src/editor/positionable_scenes_cache_manager.h>
#include "positionable_selection_pane.h"
#include <climits>

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
        const PoolStringArray &paths{positionable_set->get_path_groups()};
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
    refresh_button->connect("pressed", this, "_refresh_current_set");
}

void PositionableSelectionPane::_select_item_from_path_selector(int index) {
    const Vector<String>& scenes_paths{
        positionable_set->get_scene_paths_for_group(path_selector->get_item_text(index))
    };
    PositionableScenesCacheManager::get_instance().clear();
    item_list->clear();
    int scene_count{scenes_paths.size()};
    PositionableScenesCacheManager::get_instance().start_adding(scene_count);
    for (int i = 0; i < scene_count; ++i) {
        const Ref<PackedScene>& positionable_scene{ResourceLoader::load(scenes_paths[i])};
        StringName path{positionable_scene->get_path()};

        PositionableScenesCacheManager::get_instance().add_scene(item_list->get_item_count(), positionable_scene);
        Ref<Texture> icon_texture{PositionableScenesCacheManager::get_instance().get_icon(item_list->get_item_count())};
        item_list->add_item(positionable_scene->get_path(), icon_texture);
    }
    PositionableScenesCacheManager::get_instance().end_adding();
    
    const Vector<resource::RemovedSetElement>& removed_elements{
        positionable_set->get_removed_elements()
    };
    for (int i = 0; i < removed_elements.size(); ++i) {
        const resource::RemovedSetElement& element{removed_elements[i]};
        item_list->add_item(vformat("REMOVED ! %s", element.element_path));
        int last_item_id{item_list->get_item_count() - 1};
        item_list->set_item_metadata(last_item_id, element.id);
        item_list->set_item_custom_bg_color(last_item_id, Color(184, 40, 0));
    }
}

void PositionableSelectionPane::_refresh_current_set() {
    if (positionable_set.is_valid()) {
        positionable_set->refresh_set();
    }
}

PositionableSelectionPane::PositionableSelectionPane() : VSplitContainer(), top_container(memnew(HSplitContainer)),
                                                         path_selector(memnew(OptionButton)), refresh_button(memnew(Button)),
                                                         item_list(memnew(ItemList)), positionable_set() {
    refresh_button->set_text("refresh");
    add_child(top_container);
    top_container->add_child(path_selector);
    top_container->add_child(refresh_button);
    top_container->set_split_offset(INT_MAX);
    top_container->clamp_split_offset();
    top_container->set_dragger_visibility(DraggerVisibility::DRAGGER_HIDDEN);
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
