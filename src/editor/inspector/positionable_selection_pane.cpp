#ifdef TOOLS_ENABLED

#include <scene/main/viewport.h>
#include <editor/editor_node.h>
#include <modules/isometric_maps/src/editor/positionable_scenes_cache_manager.h>
#include "positionable_selection_pane.h"
#include <climits>
#include <modules/isometric_maps/src/editor/isometric_editor_plugin.h>

using namespace editor::inspector;

void PositionableSelectionPane::set_positionable_set(const Ref<resource::PositionableSet>& set) {
    positionable_set = set;
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
        if (_item_has_metadata(i)) continue;
        item_list->set_item_icon(i, PositionableScenesCacheManager::get_instance().get_icon(i));
    }
}

void PositionableSelectionPane::_refresh_path_selector() {
    path_selector->clear();
    if (positionable_set.is_valid()) {
        positionable_set->refresh_set();
        EditorNode::get_singleton()->save_resource(positionable_set);
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
    const String& selected_path_group{path_selector->get_item_text(index)};
    const Map<int, String>& scenes_paths{positionable_set->get_scene_paths_for_group(selected_path_group)};

    const Vector<resource::PositionableSet::RemovedElement>& removed_elements{
            positionable_set->get_removed_elements()
    };

    PositionableScenesCacheManager::get_instance().clear();
    item_list->clear();

    PositionableScenesCacheManager::get_instance().start_adding(scenes_paths.size() - removed_elements.size());

    Map<int, String>::Element* current{scenes_paths.front()};

    while (current) {
        String& positionable_path = current->value();
        FileAccessRef file_access{FileAccess::create(FileAccess::ACCESS_RESOURCES)};
        if (file_access->file_exists(positionable_path) && positionable_path.find(selected_path_group) >= 0) {
            const Ref<PackedScene>& positionable_scene{ResourceLoader::load(positionable_path)};
            const String& path{positionable_scene->get_path()};

            PositionableScenesCacheManager::get_instance().add_scene(item_list->get_item_count(), positionable_scene);
            Ref<Texture> icon_texture{
                    PositionableScenesCacheManager::get_instance().get_icon(item_list->get_item_count())};
            item_list->add_item(path, icon_texture);
        }

        current = current->next();
    }

    PositionableScenesCacheManager::get_instance().end_adding();

    for (int i = 0; i < removed_elements.size(); ++i) {
        const resource::PositionableSet::RemovedElement& element{removed_elements[i]};
        item_list->add_item(element.element_path);
        int last_item_id{item_list->get_item_count() - 1};
        item_list->set_item_metadata(last_item_id, element.id);
        item_list->set_item_custom_bg_color(last_item_id, Color(0.65, 0.14, 0));
        item_list->set_item_icon(
                last_item_id,
                IsometricEditorPlugin::get_instance()->get_editor_interface()->get_base_control()->get_icon("Remove", "EditorIcons")
        );
    }
}

void PositionableSelectionPane::_refresh_current_set() {
    _refresh_path_selector();
}

void PositionableSelectionPane::_on_item_selected(int index) {
    if (_item_has_metadata(index)) {
        FixSetDialog* fix_set_dialog = editor::IsometricEditorPlugin::get_instance()->get_fix_set_dialog();
        fix_set_dialog->reset();
        fix_set_dialog->setup(positionable_set);
        fix_set_dialog->show_modal(true);
        fix_set_dialog->popup_centered_ratio(0.75);
    }
}

bool PositionableSelectionPane::_item_has_metadata(int index) {
    return item_list->get_item_metadata(index).get_type() != Variant::NIL;
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

    item_list->connect("item_selected", this, "_on_item_selected");

    _refresh_path_selector();
}

void PositionableSelectionPane::_bind_methods() {
    ClassDB::bind_method("_refresh_path_selector", &PositionableSelectionPane::_refresh_path_selector);
    ClassDB::bind_method("_refresh_current_set", &PositionableSelectionPane::_refresh_current_set);
    ClassDB::bind_method("_select_item_from_path_selector", &PositionableSelectionPane::_select_item_from_path_selector);
    ClassDB::bind_method("set_positionable_set", &PositionableSelectionPane::set_positionable_set);
    ClassDB::bind_method("_on_item_selected", &PositionableSelectionPane::_on_item_selected);
}

#endif
