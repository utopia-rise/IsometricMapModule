#ifdef TOOLS_ENABLED

#include "positionable_selection_pane.h"

#include "editor/editor_utils.h"
#include "editor/positionable_scenes_cache_manager.h"
#include "constants.h"

#include <editor/editor_node.h>
#include <scene/main/viewport.h>

using namespace editor::inspector;

void PositionableSelectionPane::set_positionable_set(const Ref<resource::PositionableSet>& set) {
    positionable_set = set;
    refresh_path_selector();
}

int PositionableSelectionPane::get_selected_positionable_id() const {
    const Vector<int>& selected_items {item_list->get_selected_items()};
    if (selected_items.is_empty()) { return resource::PositionableSet::NONE_POSITIONABLE_ID; }
    if (auto* metadata {Object::cast_to<PositionableItemListMetadata>(item_list->get_item_metadata(selected_items[0]))}) {
        return metadata->positionable_id;
    }
    return resource::PositionableSet::NONE_POSITIONABLE_ID;
}

void PositionableSelectionPane::refresh_path_selector() {
    category_selector->clear();

    if (positionable_set.is_null()) {
        item_list->clear();
        return;
    }

    EditorNode::get_singleton()->save_resource(positionable_set);
    const PackedStringArray & paths {positionable_set->get_categories()};
    for (int i = 0; i < paths.size(); ++i) {
        category_selector->add_item(paths[i]);
    }
    _select_item_from_path_selector(category_selector->get_selected());
}

void PositionableSelectionPane::_notification(int notif) {
    if (notif == NOTIFICATION_READY) { _ready(); }
}

void PositionableSelectionPane::_ready() {
    category_selector->connect("item_selected", Callable(this, "_select_item_from_path_selector"));
    refresh_button->connect("pressed", Callable(this, "refresh_path_selector"));
}

void PositionableSelectionPane::_select_item_from_path_selector(int index) {
    PositionableScenesCacheManager::get_instance().clear(this);
    item_list->clear();

    if (index < 0) { return; }

    const String& selected_category {category_selector->get_item_text(index)};

    EditorUtils::refresh_item_list_containing_tiles_for_category(selected_category, item_list, positionable_set, this);
}

void PositionableSelectionPane::_refresh_icons() {
    EditorUtils::refresh_positionable_icons_for_item_list(this->item_list, this);
}

PositionableSelectionPane::PositionableSelectionPane() :
  top_container(memnew(HSplitContainer)),
  category_selector(memnew(OptionButton)),
  refresh_button(memnew(Button)),
  item_list(memnew(ItemList))
  {
    refresh_button->set_text("refresh");
    add_child(top_container);
    top_container->add_child(category_selector);
    top_container->add_child(refresh_button);
    top_container->set_split_offset(Constants::int_max);
    top_container->clamp_split_offset();
    top_container->set_dragger_visibility(DraggerVisibility::DRAGGER_HIDDEN);
    add_child(item_list);

    PositionableScenesCacheManager::get_instance().register_control(this, "_refresh_icons");

    refresh_path_selector();
}

PositionableSelectionPane::~PositionableSelectionPane() {
    PositionableScenesCacheManager::get_instance().unregister_control(this);
}

void PositionableSelectionPane::_bind_methods() {
    ClassDB::bind_method("refresh_path_selector", &PositionableSelectionPane::refresh_path_selector);
    ClassDB::bind_method("_select_item_from_path_selector", &PositionableSelectionPane::_select_item_from_path_selector);
    ClassDB::bind_method("set_positionable_set", &PositionableSelectionPane::set_positionable_set);

    ClassDB::bind_method("_refresh_icons", &PositionableSelectionPane::_refresh_icons);
}

#endif
