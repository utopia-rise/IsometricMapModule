#ifdef TOOLS_ENABLED

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
    const resource::PositionableSet::PositionableSceneStorage& storage{
        positionable_set->get_storage_for_path(path_selector->get_item_text(index))
    };
    _full_inspector_update(map_inspector, storage.maps);
    _full_inspector_update(positionable_inspector, storage.positionables);
}

void PositionableSelectionPane::_full_inspector_update(PositionableSetInspector* inspector,
                                                       const Vector<Ref<PackedScene>>& scenes) {
    inspector->clear();
    for (int i = 0; i < scenes.size(); ++i) {
        inspector->update_for_positionable(scenes[i]);
    }
}

PositionableSelectionPane::PositionableSelectionPane() : VSplitContainer(), path_selector(memnew(OptionButton)),
                                                         positionable_set(), selectors_container(memnew(HSplitContainer)),
                                                         map_inspector(memnew(PositionableSetInspector("Maps"))),
                                                         positionable_inspector(memnew(PositionableSetInspector("Tiles"))) {
    add_child(path_selector);
    selectors_container->add_child(map_inspector);
    selectors_container->add_child(positionable_inspector);
    add_child(selectors_container);
    _refresh_path_selector();
}

void PositionableSelectionPane::_bind_methods() {
    ClassDB::bind_method("_refresh_path_selector", &PositionableSelectionPane::_refresh_path_selector);
    ClassDB::bind_method("_select_item_from_path_selector", &PositionableSelectionPane::_select_item_from_path_selector);
    ClassDB::bind_method("set_positionable_set", &PositionableSelectionPane::set_positionable_set);
}

#endif
