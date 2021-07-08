#ifdef TOOLS_ENABLED

#include <scene/gui/button.h>
#include <scene/gui/label.h>
#include <scene/gui/item_list.h>
#include <scene/gui/option_button.h>
#include <core/os/file_access.h>
#include <modules/isometric_maps/src/editor/positionable_set_editor_plugin.h>
#include <modules/isometric_maps/src/editor/positionable_scenes_cache_manager.h>
#include <modules/isometric_maps/src/editor/editor_utils.h>
#include <editor/editor_node.h>
#include <modules/isometric_maps/src/editor/isometric_editor_plugin.h>
#include "positionable_set_editor.h"

using namespace editor::inspector;

void PositionableSetEditor::set_positionable_set(const Ref<resource::PositionableSet>& p_positionable_set) {
    current_set = p_positionable_set;
    _refresh_categories();
}

PositionableSetEditor::PositionableSetEditor() : VBoxContainer(), category_selector(memnew(OptionButton)),
                                                 contained_tiles_list(memnew(ItemList)),
                                                 fix_path_button(memnew(Button)),
                                                 add_category_dialog(memnew(WindowDialog)),
                                                 add_category_dialog_line_edit(memnew(LineEdit)),
                                                 file_dialog(memnew(FileDialog)),
                                                 fix_path_dialog(memnew(FileDialog)),
                                                 alert_popup(memnew(AcceptDialog)),
                                                 remove_tile_alert_popup(memnew(WindowDialog)),
                                                 do_not_display_alert_remove_tile(memnew(CheckBox)),
                                                 remove_category_alert_popup(memnew(WindowDialog)),
                                                 do_not_display_alert_remove_category(memnew(CheckBox)),
                                                 current_set() {
    HBoxContainer* select_category_container{memnew(HBoxContainer)};
    Label* category_selector_label{memnew(Label)};
    category_selector_label->set_text("Categories:");
    category_selector->set_h_size_flags(SIZE_EXPAND_FILL);
    Button* add_category_button{memnew(Button)};
    add_category_button->set_text("+");
    Button* remove_category_button{memnew(Button)};
    remove_category_button->set_text("-");

    select_category_container->add_child(category_selector_label);
    select_category_container->add_child(category_selector);
    select_category_container->add_child(add_category_button);
    select_category_container->add_child(remove_category_button);

    add_child(select_category_container);

    Label* contained_tiles_label{memnew(Label)};
    contained_tiles_label->set_text("Contained positionables:");

    add_child(contained_tiles_label);

    contained_tiles_list->set_h_size_flags(SIZE_EXPAND_FILL);
    contained_tiles_list->set_v_size_flags(SIZE_EXPAND_FILL);

    add_child(contained_tiles_list);

    HBoxContainer* add_and_remove_tiles_container{memnew(HBoxContainer)};

    fix_path_button->set_text("Fix !");
    fix_path_button->set_visible(false);
    add_and_remove_tiles_container->add_child(fix_path_button);

    Button* add_positionable_button{memnew(Button)};
    add_positionable_button->set_text("+");

    add_and_remove_tiles_container->add_child(add_positionable_button);

    Button* remove_positionable_button{memnew(Button)};
    remove_positionable_button->set_text("-");

    add_and_remove_tiles_container->add_child(remove_positionable_button);
    add_and_remove_tiles_container->set_h_size_flags(SIZE_SHRINK_END);

    add_child(add_and_remove_tiles_container);

    Button* save_button{memnew(Button)};
    save_button->set_text("Save");

    add_child(save_button);

    add_category_dialog->set_title("Add category");
    VBoxContainer* add_category_dialog_container{memnew(VBoxContainer)};
    add_category_dialog_line_edit->set_h_size_flags(SIZE_EXPAND_FILL);
    Button* add_category_dialog_ok_button{memnew(Button)};
    add_category_dialog_ok_button->set_text("Ok!");
    add_category_dialog_ok_button->set_h_size_flags(SIZE_SHRINK_CENTER);
    add_category_dialog_container->add_child(add_category_dialog_line_edit);
    add_category_dialog_container->add_child(add_category_dialog_ok_button);
    add_category_dialog_container->set_anchor(Margin::MARGIN_RIGHT, 0.99);
    add_category_dialog_container->set_anchor(Margin::MARGIN_BOTTOM, 0.99);
    add_category_dialog_container->set_anchor(Margin::MARGIN_LEFT, 0.01);
    add_category_dialog_container->set_anchor(Margin::MARGIN_TOP, 0.01);
    add_category_dialog->add_child(add_category_dialog_container);

    add_child(add_category_dialog);
    add_category_dialog->set_visible(false);

    add_child(file_dialog);
    Vector<String> file_dialog_filters;
    file_dialog_filters.push_back("*.tscn");
    file_dialog->set_filters(file_dialog_filters);
    file_dialog->set_visible(false);
    file_dialog->set_mode(FileDialog::MODE_OPEN_ANY);

    add_child(fix_path_dialog);
    Vector<String> fix_path_dialog_filters;
    fix_path_dialog_filters.push_back("*.tscn");
    fix_path_dialog->set_filters(fix_path_dialog_filters);
    fix_path_dialog->set_visible(false);
    fix_path_dialog->set_mode(FileDialog::MODE_OPEN_FILE);

    add_child(alert_popup);
    alert_popup->set_visible(false);

    Button* remove_tile_alert_ok_button{_generate_alert_remove_dialog(remove_tile_alert_popup, do_not_display_alert_remove_tile)};
    add_child(remove_tile_alert_popup);
    remove_tile_alert_popup->set_visible(false);

    Button* remove_category_alert_ok_button{_generate_alert_remove_dialog(remove_category_alert_popup, do_not_display_alert_remove_category)};
    add_child(remove_category_alert_popup);
    remove_category_alert_popup->set_visible(false);

    category_selector->connect("item_selected", this, "_on_category_selected");
    add_category_button->connect("pressed", this, "_on_add_category_button");
    remove_category_button->connect("pressed", this, "_on_remove_category_button");
    add_category_dialog_ok_button->connect("pressed", this, "_on_add_category_dialog_ok_button");
    remove_category_alert_ok_button->connect("pressed", this, "_on_remove_category_alert_ok_button");

    add_positionable_button->connect("pressed", this, "_on_add_positionable_button");
    remove_positionable_button->connect("pressed", this, "_on_remove_positionable_button");
    remove_tile_alert_ok_button->connect("pressed", this, "_on_remove_tile_alert_ok_button");

    fix_path_button->connect("pressed", this, "_on_fix_path_button");

    file_dialog->connect("dir_selected", this, "_on_file_dialog_file_or_dir_selected");
    file_dialog->connect("file_selected", this, "_on_file_dialog_file_or_dir_selected");

    fix_path_dialog->connect("file_selected", this, "_on_fix_path_dialog_file_selected");

    contained_tiles_list->connect("item_selected", this, "_on_positionable_selected");

    save_button->connect("pressed", this, "_on_save_button");

    PositionableScenesCacheManager::get_instance().register_control(
            this,
            [this]() { EditorUtils::refresh_positionable_icons_for_item_list(this->contained_tiles_list, this);}
    );
}

PositionableSetEditor::~PositionableSetEditor() {
    PositionableScenesCacheManager::get_instance().unregister_control(this);
}

void PositionableSetEditor::_on_add_category_button() {
    add_category_dialog->popup_centered_ratio(0.5);
}

void PositionableSetEditor::_on_add_category_dialog_ok_button() {
    add_category_dialog->set_visible(false);
    if (current_set.is_valid()) {
        const String& category_to_add{add_category_dialog_line_edit->get_text()};
        if (!current_set->add_category(category_to_add)) {
            alert_popup->set_text("This category already exists !");
            alert_popup->popup_centered();
            return;
        }
        category_selector->add_item(category_to_add);
    }
    add_category_dialog_line_edit->set_text("");
}

void PositionableSetEditor::_on_remove_category_button() {
    if (_popup_if_no_category_selected()) {
        return;
    }

    if (!do_not_display_alert_remove_category->is_pressed()) {
        remove_category_alert_popup->popup_centered_ratio(0.4);
        return;
    }

    _remove_selected_category();
}

void PositionableSetEditor::_on_remove_category_alert_ok_button() {
    remove_category_alert_popup->set_visible(false);
    _remove_selected_category();
}

void PositionableSetEditor::_on_add_positionable_button() {
    if (_popup_if_no_category_selected()) {
        return;
    }
    file_dialog->popup_centered_ratio(0.75);
}

void PositionableSetEditor::_on_file_dialog_file_or_dir_selected(const String& path) {
    List<String> positionables_in_path;
    EditorUtils::find_all_positionables_in_path(path, &positionables_in_path);

    for (int i = 0; i < positionables_in_path.size(); ++i) {
        const String& positionable_path{positionables_in_path[i]};
        const String& category{category_selector->get_text()};
        current_set->insert_positionable_if_not_present(category, positionable_path);
    }

    _on_category_selected(category_selector->get_selected());
}

void PositionableSetEditor::_on_remove_positionable_button() {
    if (_popup_if_no_category_selected()) {
        return;
    }

    if (contained_tiles_list->get_selected_items().empty()) {
        alert_popup->set_text("You must select a tile !");
        alert_popup->popup_centered();
        return;
    }

    if (!do_not_display_alert_remove_tile->is_pressed()) {
        remove_tile_alert_popup->popup_centered_ratio(0.4);
        return;
    }

    _remove_selected_positionable();
}

void PositionableSetEditor::_on_remove_tile_alert_ok_button() {
    remove_tile_alert_popup->set_visible(false);
    _remove_selected_positionable();
}

void PositionableSetEditor::_on_fix_path_button() {
    fix_path_dialog->popup_centered_ratio();
}

void PositionableSetEditor::_on_fix_path_dialog_file_selected(const String& path) {
    List<String> positionables_in_path;
    EditorUtils::find_all_positionables_in_path(path, &positionables_in_path);

    if (positionables_in_path.empty()) return;

    if (auto* metadata{Object::cast_to<PositionableItemListMetadata>(
            contained_tiles_list->get_item_metadata(contained_tiles_list->get_selected_items()[0])
    )}) {
        if (!current_set->add_or_update_positionable(metadata->positionable_id, positionables_in_path[0])) {
            alert_popup->set_text("Tile already exists in set !");
            alert_popup->popup_centered();
        }
    }

    _on_category_selected(category_selector->get_selected());
}

void PositionableSetEditor::_refresh_categories() {
    category_selector->clear();
    if (current_set.is_valid()) {
        const PoolStringArray &paths{current_set->get_categories()};
        for (int i = 0; i < paths.size(); ++i) {
            category_selector->add_item(paths[i]);
        }
    }

    _on_category_selected(category_selector->get_selected());
}

void PositionableSetEditor::_on_category_selected(int index) {
    PositionableScenesCacheManager::get_instance().clear(this);
    contained_tiles_list->clear();

    if (index < 0) {
        return;
    }

    const String& selected_category{category_selector->get_item_text(index)};

    EditorUtils::refresh_item_list_containing_tiles_for_category(
            selected_category,
            contained_tiles_list,
            current_set,
            this
    );
}

void PositionableSetEditor::_on_positionable_selected(int index) {
    fix_path_button->set_visible(!EditorUtils::item_from_list_has_valid_tile_metadata(contained_tiles_list, index));
}

void PositionableSetEditor::_on_save_button() {
    if (current_set.is_valid()) {
        EditorNode::get_singleton()->save_resource(current_set);
    }
}

bool PositionableSetEditor::_popup_if_no_category_selected() {
    int selected_category{category_selector->get_selected()};
    if (selected_category < 0 || category_selector->get_item_text(selected_category).empty()) {
        alert_popup->set_text("You must select a category !");
        alert_popup->popup_centered();
        return true;
    }
    return false;
}

void PositionableSetEditor::_remove_selected_positionable() {
    int selected_item_index{contained_tiles_list->get_selected_items()[0]};
    Ref<PositionableItemListMetadata> metadata{contained_tiles_list->get_item_metadata(selected_item_index)};
    current_set->remove_positionable(metadata->positionable_id);
    contained_tiles_list->clear();
    _on_category_selected(category_selector->get_selected());
}

void PositionableSetEditor::_remove_selected_category() {
    current_set->remove_category(category_selector->get_text());
    _refresh_categories();
}

Button* PositionableSetEditor::_generate_alert_remove_dialog(WindowDialog* dialog,
                                                             CheckBox* do_not_display_alert_check_box) {
    dialog->set_title("Remove from positionable set");
    VBoxContainer* remove_alert_container{memnew(VBoxContainer)};
    Label* alert_remove_text{memnew(Label)};
    alert_remove_text->set_text("You're going to remove tile(s) from set.\nMake sure those tiles are not used anymore in maps.\n"
                                "If those are still in use, this will break you're map and you'll have to fix it.");
    alert_remove_text->set_h_size_flags(SIZE_EXPAND_FILL);
    alert_remove_text->set_v_size_flags(SIZE_EXPAND_FILL);
    remove_alert_container->add_child(alert_remove_text);
    do_not_display_alert_check_box->set_text("Do not display this message anymore.");
    remove_alert_container->add_child(do_not_display_alert_check_box);
    Button* remove_alert_ok_button{memnew(Button)};
    remove_alert_ok_button->set_text("Remove !");
    remove_alert_ok_button->set_h_size_flags(SIZE_SHRINK_CENTER);
    remove_alert_container->add_child(remove_alert_ok_button);
    remove_alert_container->set_anchor(Margin::MARGIN_RIGHT, 0.99);
    remove_alert_container->set_anchor(Margin::MARGIN_BOTTOM, 0.99);
    remove_alert_container->set_anchor(Margin::MARGIN_LEFT, 0.01);
    remove_alert_container->set_anchor(Margin::MARGIN_TOP, 0.01);
    dialog->add_child(remove_alert_container);
    return remove_alert_ok_button;
}

void PositionableSetEditor::_bind_methods() {
    ClassDB::bind_method("_on_add_category_button", &PositionableSetEditor::_on_add_category_button);
    ClassDB::bind_method("_on_add_category_dialog_ok_button", &PositionableSetEditor::_on_add_category_dialog_ok_button);
    ClassDB::bind_method("_on_remove_category_button", &PositionableSetEditor::_on_remove_category_button);
    ClassDB::bind_method("_on_remove_category_alert_ok_button", &PositionableSetEditor::_on_remove_category_alert_ok_button);
    ClassDB::bind_method("_on_add_positionable_button", &PositionableSetEditor::_on_add_positionable_button);
    ClassDB::bind_method("_on_file_dialog_file_or_dir_selected", &PositionableSetEditor::_on_file_dialog_file_or_dir_selected);
    ClassDB::bind_method("_on_remove_positionable_button", &PositionableSetEditor::_on_remove_positionable_button);
    ClassDB::bind_method("_on_remove_tile_alert_ok_button", &PositionableSetEditor::_on_remove_tile_alert_ok_button);
    ClassDB::bind_method("_on_fix_path_button", &PositionableSetEditor::_on_fix_path_button);
    ClassDB::bind_method("_on_fix_path_dialog_file_selected", &PositionableSetEditor::_on_fix_path_dialog_file_selected);
    ClassDB::bind_method("_on_category_selected", &PositionableSetEditor::_on_category_selected);
    ClassDB::bind_method("_on_positionable_selected", &PositionableSetEditor::_on_positionable_selected);
    ClassDB::bind_method("_on_save_button", &PositionableSetEditor::_on_save_button);
}

#endif
