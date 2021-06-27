#ifdef TOOLS_ENABLED

#include <scene/gui/label.h>
#include <scene/gui/separator.h>
#include <modules/isometric_maps/src/editor/editor_utils.h>
#include <modules/isometric_maps/src/editor/isometric_editor_plugin.h>
#include "fix_set_dialog.h"

using namespace editor;
using namespace editor::inspector;

void FixSetDialog::setup(Ref<resource::PositionableSet> positionable_set) {
    current_positionable_set = positionable_set;
    const Vector<resource::RemovedSetElement>& removed_elements{positionable_set->get_removed_elements()};
    for (int i = 0; i < removed_elements.size(); ++i) {
        const resource::RemovedSetElement& removed_element{removed_elements[i]};
        removed_elements_item_list->add_item(removed_element.element_path);
        removed_elements_item_list->set_item_metadata(removed_elements_item_list->get_item_count() - 1, removed_element.id);
    }
    Map<int, String>::Element* current{positionable_set->get_present_scenes_iterator()};
    while (current) {
        already_presents.push_back(current->value());
        current = current->next();
    }
}

void FixSetDialog::reset() {
    current_positionable_set.unref();
    current_positionable_set = Ref<resource::PositionableSet>();
    add_group_path_line_edit->clear();
    added_group_paths_item_list->clear();
    removed_elements_item_list->clear();
    new_elements_item_list->clear();
    fix_recap_item_list->clear();
}

void FixSetDialog::_on_add_group_path_button() {
    const String& group_path{add_group_path_line_edit->get_text()};
    const PoolStringArray& existing_group_path{current_positionable_set->get_path_groups()};
    for (int i = 0; i < existing_group_path.size(); ++i) {
        if (existing_group_path[i].find(group_path) >= 0) {
            //TODO: throw popup saying "Cannot add an existing group path"
            return;
        }
    }

    added_group_paths_item_list->add_item(group_path);

    List<String> scene_paths_in_new_group;
    EditorUtils::find_all_positionables_in_path(group_path, &scene_paths_in_new_group);
    for (int i = 0; i < scene_paths_in_new_group.size(); ++i) {
        new_elements_item_list->add_item(scene_paths_in_new_group[i]);
        new_elements_item_list->set_item_metadata(new_elements_item_list->get_item_count() - 1, group_path);
    }
    add_group_path_line_edit->clear();
}

void FixSetDialog::_on_remove_button() {
    if (removed_elements_item_list->get_selected_items().size() != 1) {
        //TODO : Display popup to say require to select
        return;
    }
    int selected_index{removed_elements_item_list->get_selected_items()[0]};
    const String& item_text{removed_elements_item_list->get_item_text(selected_index)};
    int item_id{removed_elements_item_list->get_item_metadata(selected_index)};

    Ref<AssociationMetadata> association_metadata;
    association_metadata.instance();
    association_metadata->set_id(item_id);
    association_metadata->set_removed_path(item_text);

    removed_elements_item_list->remove_item(selected_index);

    _add_new_association(association_metadata);
}

void FixSetDialog::_on_new_association_button() {
    if (removed_elements_item_list->get_selected_items().size() != 1 && new_elements_item_list->get_selected_items().size() != 1) {
        //TODO: Display popup to say require to select
        return;
    }
    int selected_removed_element{removed_elements_item_list->get_selected_items()[0]};
    const String& removed_element_text{removed_elements_item_list->get_item_text(selected_removed_element)};
    int item_id{removed_elements_item_list->get_item_metadata(selected_removed_element)};

    int selected_new_element{new_elements_item_list->get_selected_items()[0]};
    const String& new_element_text{new_elements_item_list->get_item_text(selected_new_element)};
    const String& new_element_path_group{new_elements_item_list->get_item_metadata(selected_new_element)};

    Ref<AssociationMetadata> association_metadata;
    association_metadata.instance();
    association_metadata->set_id(item_id);
    association_metadata->set_removed_path(removed_element_text);
    association_metadata->set_new_path(new_element_text);
    association_metadata->set_path_group(new_element_path_group);

    removed_elements_item_list->remove_item(selected_removed_element);
    new_elements_item_list->remove_item(selected_new_element);

    _add_new_association(association_metadata);
}

void FixSetDialog::_on_revert_association_button() {
    const Vector<int>& selected_items{fix_recap_item_list->get_selected_items()};
    if (selected_items.size() != 1) {
        return;
    }
    int selected_association{selected_items[0]};
    Ref<AssociationMetadata> association_metadata{fix_recap_item_list->get_item_metadata(selected_association)};

    const String& removed_path{association_metadata->get_removed_path()};
    int id{association_metadata->get_id()};
    removed_elements_item_list->add_item(removed_path);
    removed_elements_item_list->set_item_metadata(removed_elements_item_list->get_item_count() - 1, id);

    const String& new_path{association_metadata->get_new_path()};
    if (!new_path.empty()) {
        new_elements_item_list->add_item(new_path);
    }
    fix_recap_item_list->remove_item(selected_association);
    validate_button->set_disabled(true);
}

void FixSetDialog::_on_validate_button() {
    current_positionable_set->clear_removed_elements();
    for (int i = 0; i < fix_recap_item_list->get_item_count(); ++i) {
        Ref<AssociationMetadata> association_metadata{fix_recap_item_list->get_item_metadata(i)};
        const String& new_path{association_metadata->get_new_path()};
        if (new_path.empty()) {
            continue;
        }

        const String& path_group{association_metadata->get_path_group()};
        if (!current_positionable_set->has_path_group(path_group)) {
            current_positionable_set->add_path_group(path_group);
        }
        current_positionable_set->add_positionable(association_metadata->get_id(), path_group, new_path);
    }
    current_positionable_set->refresh_set();
    EditorNode::get_singleton()->save_resource(current_positionable_set);
    set_visible(false);
    reset();
}

void FixSetDialog::_add_new_association(const Ref<AssociationMetadata>& association_metadata) {
    const String& new_path{association_metadata->get_new_path()};
    String text;
    String icon_name;
    Color bg_color;
    if (new_path.empty()) {
        text = association_metadata->get_removed_path();
        icon_name = "Remove";
        bg_color = Color(0.65, 0.14, 0);
    } else {
        text = vformat(
                "%s => %s",
                association_metadata->get_removed_path(),
                association_metadata->get_new_path()
        );
        icon_name = "Add";
        bg_color = Color(0, 0.14, 0.65);
    }
    fix_recap_item_list->add_item(text);
    int new_item_index{fix_recap_item_list->get_item_count() - 1};
    fix_recap_item_list->set_item_icon(
            new_item_index,
            IsometricEditorPlugin::get_instance()->get_editor_interface()->get_base_control()->get_icon(icon_name, "EditorIcons")
    );
    fix_recap_item_list->set_item_custom_bg_color(new_item_index, bg_color);
    fix_recap_item_list->set_item_metadata(new_item_index, association_metadata);

    if (removed_elements_item_list->get_item_count() == 0) {
        validate_button->set_disabled(false);
    }
}

FixSetDialog::FixSetDialog() : WindowDialog(), add_group_path_line_edit(memnew(LineEdit)),
                               added_group_paths_item_list(memnew(ItemList)), removed_elements_item_list(memnew(ItemList)),
                               new_elements_item_list(memnew(ItemList)), fix_recap_item_list(memnew(ItemList)),
                               validate_button(memnew(Button)), current_positionable_set(), already_presents() {
    set_title("Fix positionable set");
    Button* add_group_path_button{memnew(Button)};
    add_group_path_button->set_text("Add");

    VBoxContainer* main_container{memnew(VBoxContainer)};
    main_container->add_child(memnew(Separator));

    HBoxContainer* add_group_path_container{memnew(HBoxContainer)};
    Label* add_group_path_label{memnew(Label)};
    add_group_path_label->set_text("Add group path: ");
    add_group_path_line_edit->set_h_size_flags(SizeFlags::SIZE_EXPAND_FILL);
    add_group_path_container->add_child(add_group_path_label);
    add_group_path_container->add_child(add_group_path_line_edit);
    add_group_path_container->add_child(add_group_path_button);

    main_container->add_child(add_group_path_container);
    main_container->add_child(memnew(Separator));

    VBoxContainer* added_group_paths_container{memnew(VBoxContainer)};
    Label* added_group_paths_title{memnew(Label)};
    added_group_paths_title->set_text("Looking group paths:");
    added_group_paths_container->add_child(added_group_paths_title);
    added_group_paths_item_list->set_custom_minimum_size({0, 100});
    added_group_paths_container->add_child(added_group_paths_item_list);

    main_container->add_child(added_group_paths_container);
    main_container->add_child(memnew(Separator));

    HBoxContainer* new_association_container{memnew(HBoxContainer)};
    VBoxContainer* remove_elements_container{memnew(VBoxContainer)};
    VBoxContainer* new_elements_in_group_paths_container{memnew(VBoxContainer)};

    Label* removed_elements_title{memnew(Label)};
    removed_elements_title->set_text("Removed elements:");
    Button* remove_button{memnew(Button)};
    remove_button->set_text("Remove !");
    remove_button->set_h_size_flags(SizeFlags::SIZE_SHRINK_CENTER);
    removed_elements_item_list->set_custom_minimum_size({0, 100});
    removed_elements_item_list->set_h_size_flags(SizeFlags::SIZE_EXPAND_FILL);
    remove_elements_container->set_h_size_flags(SIZE_EXPAND_FILL);
    remove_elements_container->add_child(removed_elements_title);
    remove_elements_container->add_child(removed_elements_item_list);
    remove_elements_container->add_child(remove_button);

    Label* new_elements_title{memnew(Label)};
    new_elements_title->set_text("Elements from new group path:");
    Button* new_association_button{memnew(Button)};
    new_association_button->set_text("Fix !");
    new_association_button->set_h_size_flags(SizeFlags::SIZE_SHRINK_CENTER);
    new_elements_item_list->set_custom_minimum_size({0, 100});
    new_elements_item_list->set_h_size_flags(SizeFlags::SIZE_EXPAND_FILL);
    new_elements_in_group_paths_container->set_h_size_flags(SIZE_EXPAND_FILL);
    new_elements_in_group_paths_container->add_child(new_elements_title);
    new_elements_in_group_paths_container->add_child(new_elements_item_list);
    new_elements_in_group_paths_container->add_child(new_association_button);

    new_association_container->set_h_size_flags(SIZE_EXPAND_FILL);
    new_association_container->add_child(remove_elements_container);
    new_association_container->add_child(new_elements_in_group_paths_container);

    main_container->add_child(new_association_container);
    main_container->add_child(memnew(Separator));

    VBoxContainer* fix_recap_container{memnew(VBoxContainer)};
    Label* fix_recap_title{memnew(Label)};
    fix_recap_title->set_text("Fix recap:");
    Button* remove_fix_button{memnew(Button)};
    remove_fix_button->set_text("Revert !");
    fix_recap_item_list->set_custom_minimum_size({0, 100});
    fix_recap_container->add_child(fix_recap_title);
    fix_recap_container->add_child(fix_recap_item_list);
    fix_recap_container->add_child(remove_fix_button);

    main_container->add_child(fix_recap_container);

    Separator* final_separator{memnew(Separator)};
    final_separator->set_custom_minimum_size({0, 20});
    main_container->add_child(final_separator);

    validate_button->set_h_size_flags(SizeFlags::SIZE_SHRINK_END);
    validate_button->set_text("Validate !");
    main_container->add_child(validate_button);

    main_container->set_anchor(Margin::MARGIN_RIGHT, 1);
    main_container->set_anchor(Margin::MARGIN_BOTTOM, 1);

    add_child(main_container);

    add_group_path_button->connect("pressed", this, "_on_add_group_path_button");
    remove_button->connect("pressed", this, "_on_remove_button");
    new_association_button->connect("pressed", this, "_on_new_association_button");
    remove_fix_button->connect("pressed", this, "_on_revert_association_button");
    validate_button->connect("pressed", this, "_on_validate_button");

    validate_button->set_disabled(true);
}

void FixSetDialog::_bind_methods() {
    ClassDB::bind_method("_on_add_group_path_button", &FixSetDialog::_on_add_group_path_button);
    ClassDB::bind_method("_on_remove_button", &FixSetDialog::_on_remove_button);
    ClassDB::bind_method("_on_new_association_button", &FixSetDialog::_on_new_association_button);
    ClassDB::bind_method("_on_revert_association_button", &FixSetDialog::_on_revert_association_button);
    ClassDB::bind_method("_on_validate_button", &FixSetDialog::_on_validate_button);
}

FixSetDialog::AssociationMetadata::AssociationMetadata() :Reference(), id(-1), removed_path(), new_path() {

}

int FixSetDialog::AssociationMetadata::get_id() const {
    return id;
}

const String& FixSetDialog::AssociationMetadata::get_removed_path() const {
    return removed_path;
}

const String& FixSetDialog::AssociationMetadata::get_new_path() const {
    return new_path;
}

void FixSetDialog::AssociationMetadata::set_id(int p_id) {
    id = p_id;
}

void FixSetDialog::AssociationMetadata::set_removed_path(const String& p_removed_path) {
    removed_path = p_removed_path;
}

void FixSetDialog::AssociationMetadata::set_new_path(const String& p_new_path) {
    new_path = p_new_path;
}

void FixSetDialog::AssociationMetadata::set_path_group(const String& p_path_group) {
    path_group = p_path_group;
}

const String& FixSetDialog::AssociationMetadata::get_path_group() const {
    return path_group;
}

#endif