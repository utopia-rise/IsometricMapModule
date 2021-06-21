#ifdef TOOLS_ENABLED

#include <scene/gui/label.h>
#include <scene/gui/separator.h>
#include "fix_set_dialog.h"

using namespace editor::inspector;

FixSetDialog::FixSetDialog() : add_group_path_line_edit(memnew(LineEdit)), add_group_path_button(memnew(Button)),
                added_group_paths_item_list(memnew(ItemList)), removed_elements_item_list(memnew(ItemList)),
                new_elements_item_list(memnew(ItemList)), fix_recap_item_list(memnew(ItemList)) {
    set_title("Fix positionable set");
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
    remove_fix_button->set_text("Remove !");
    fix_recap_item_list->set_custom_minimum_size({0, 100});
    fix_recap_container->add_child(fix_recap_title);
    fix_recap_container->add_child(fix_recap_item_list);
    fix_recap_container->add_child(remove_fix_button);

    main_container->add_child(fix_recap_container);

    Separator* final_separator{memnew(Separator)};
    final_separator->set_custom_minimum_size({0, 20});
    main_container->add_child(final_separator);

    Button* validate_button{memnew(Button)};
    validate_button->set_h_size_flags(SizeFlags::SIZE_SHRINK_END);
    validate_button->set_text("Validate !");
    main_container->add_child(validate_button);

    main_container->set_anchor(Margin::MARGIN_RIGHT, 1);
    main_container->set_anchor(Margin::MARGIN_BOTTOM, 1);

    add_child(main_container);
}

void FixSetDialog::_bind_methods() {

}

#endif