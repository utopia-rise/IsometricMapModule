#ifdef TOOLS_ENABLED

#include "layers_editor.h"

#include "editor/commands/add_layer_command.h"
#include "editor/commands/change_layer_name_command.h"
#include "editor/commands/revert_command.h"
#include "editor/commands/set_layer_visibility_command.h"
#include "editor/isometric_editor_plugin.h"

using namespace editor::inspector;

constexpr const char add_layer_action_name[] = "Add layer";
constexpr const char remove_layer_action_name[] = "Remove layer";
constexpr const char set_layer_visible_action_name[] = "Set layer visible";
constexpr const char change_layer_name_action_name_format[] = "Change layer name (id: %s)";

LayersEditor::LayersEditor() {
    HBoxContainer* top_bar {memnew(HBoxContainer)};
    layer_line_edit->set_h_size_flags(SizeFlags::SIZE_EXPAND_FILL);
    Button* add_button {memnew(Button)};
    add_button->set_text("+");
    top_bar->add_child(layer_line_edit);
    top_bar->add_child(add_button);
    add_button->connect(SNAME("pressed"), callable_mp(this, &LayersEditor::_add_layer));
    add_child(top_bar);
    ScrollContainer* scroll_container {memnew(ScrollContainer)};
    layer_controls_container->set_columns(5);
    layer_controls_container->set_h_size_flags(SizeFlags::SIZE_EXPAND_FILL);
    scroll_container->add_child(layer_controls_container);
    add_child(scroll_container);

    current_layer_button_group = Ref<ButtonGroup>();
    current_layer_button_group.instantiate();

    refresh();

    scroll_container->set_h_size_flags(SizeFlags::SIZE_EXPAND_FILL);
    scroll_container->set_v_size_flags(SizeFlags::SIZE_EXPAND_FILL);
    set_h_size_flags(SizeFlags::SIZE_EXPAND_FILL);
    set_v_size_flags(SizeFlags::SIZE_EXPAND_FILL);

    remove_layer_popup->set_exclusive(true);
    remove_layer_popup->add_child(remove_layer_popup_label);
    add_child(remove_layer_popup);
}

void LayersEditor::refresh() {
    TypedArray<Node> elements {layer_controls_container->get_children()};
    for (int i = 0; i < elements.size(); ++i) {
        Node* child = Object::cast_to<Node>(elements[i].operator Object*());
        layer_controls_container->remove_child(child);
        child->queue_free();
    }

    if (node::IsometricMap* current_map = IsometricEditorPlugin::get_instance()->get_selected_map()) {
        Label* is_current_layer_label {memnew(Label)};
        is_current_layer_label->set_text("current");
        Label* is_visible_label {memnew(Label)};
        is_visible_label->set_text("visible");
        Label* layer_name_label {memnew(Label)};
        layer_name_label->set_text("Layer name");
        Label* color_label {memnew(Label)};
        color_label->set_text("Color");
        Label* remove_label {memnew(Label)};
        remove_label->set_text("Remove");
        layer_controls_container->add_child(is_current_layer_label);
        layer_controls_container->add_child(layer_name_label);
        layer_controls_container->add_child(color_label);
        layer_controls_container->add_child(is_visible_label);
        layer_controls_container->add_child(remove_label);

        uint32_t last_layer_edited {
            current_map->get_meta(
              node::IsometricMap::LAST_EDITED_LAYER_META_NAME,
              node::IsometricMap::DEFAULT_LAYER_ID
            )
        };

        const Dictionary& layers = current_map->get_layers();
        Array ids = layers.keys();
        Array layer_names = layers.values();
        for (int i = 0; i < layers.size(); ++i) {
            const String& layer_name {layer_names[i]};
            const uint32_t layer_id {ids[i]};
            CheckBox* current_layer_check_box {memnew(CheckBox)};
            current_layer_check_box->connect(
              SNAME("pressed"),
              callable_mp(this, &LayersEditor::_set_current_layer).bind(layer_id)
            );
            current_layer_check_box->set_button_group(current_layer_button_group);
            layer_controls_container->add_child(current_layer_check_box);

            if (layer_name == node::IsometricMap::DEFAULT_LAYER_NAME) {
                Label* current_layer_name_label {memnew(Label)};
                current_layer_name_label->set_text(layer_name);
                layer_controls_container->add_child(current_layer_name_label);
            } else {
                LineEdit* current_layer_name_line_edit {memnew(LineEdit)};
                current_layer_name_line_edit->set_text(layer_name);
                current_layer_name_line_edit->connect(
                  SNAME("text_changed"),
                  callable_mp(this, &LayersEditor::_on_layer_name_changed).bind(layer_id)
                );
                layer_controls_container->add_child(current_layer_name_line_edit);
            }

            ColorPickerButton* color_picker_button {memnew(ColorPickerButton)};
            color_picker_button->connect(
              SNAME("color_changed"),
              callable_mp(this, &LayersEditor::_layer_color_changed).bind(layer_id)
            );
            const Variant& layer_color {
              current_map->get_meta(vformat(node::IsometricMap::LAYER_COLOR_META_NAME_FORMAT, layer_id), Color())
            };
            color_picker_button->set_pick_color(layer_color);
            current_map->set_layer_color(layer_id, layer_color);
            layer_controls_container->add_child(color_picker_button);
            CheckBox* visible_check_box {memnew(CheckBox)};
            visible_check_box->set_pressed(true);
            visible_check_box->connect(
              SNAME("pressed"),
              callable_mp(this, &LayersEditor::_set_layer_visible).bind(layer_id, visible_check_box)
            );
            layer_controls_container->add_child(visible_check_box);
            Button* layer_remove_button {memnew(Button)};
            layer_remove_button->set_text("-");
            layer_remove_button->connect(
              SNAME("pressed"),
              callable_mp(this, &LayersEditor::_on_remove_layer_button).bind(layer_id, layer_name)
            );
            layer_controls_container->add_child(layer_remove_button);

            current_layer_check_box->set_pressed(layer_id == last_layer_edited);
        }
    }
}

void LayersEditor::_add_layer() {
    Vector<Ref<commands::Command<node::IsometricMap>>> commands;
    Ref<commands::AddLayerCommand> add_layer_command;
    add_layer_command.instantiate();
    add_layer_command->set_layer_name(layer_line_edit->get_text());

    add_layer_command->add_hook(callable_mp(this, &LayersEditor::refresh));

    commands.push_back(add_layer_command);

    commands::emitters::CommandToActionTransformer action_transformer;
    action_transformer.transform<node::IsometricMap, add_layer_action_name>(commands, IsometricEditorPlugin::get_instance()->get_selected_map());

    layer_line_edit->clear();
}

void LayersEditor::_on_remove_layer_button(const uint32_t p_layer_id, const String& p_layer_name) {
    remove_layer_popup_label->set_text(
      vformat(
        "You're going to remove layer %s.\n"
        "This will remove all tiles included in layer from map.\n"
        "Please confirm removal.",
        p_layer_name
      )
    );
    Callable callable {callable_mp(this, &LayersEditor::_remove_layer)};
    if (remove_layer_popup->is_connected(SNAME("confirmed"), callable)) {
        remove_layer_popup->disconnect(SNAME("confirmed"), callable);
    }
    remove_layer_popup->connect(SNAME("confirmed"), callable.bind(p_layer_id, p_layer_name));
    remove_layer_popup->popup_centered();
}

void LayersEditor::_on_layer_name_changed(const String& p_layer_name, uint32_t p_layer_id) { // NOLINT(*-convert-member-functions-to-static)
    if (node::IsometricMap* current_map {IsometricEditorPlugin::get_instance()->get_selected_map()}) {
        Vector<Ref<commands::Command<node::IsometricMap>>> commands;

        Ref<commands::ChangeLayerNameCommand> change_layer_name_command;
        change_layer_name_command.instantiate();
        change_layer_name_command->set_layer_id(p_layer_id);
        change_layer_name_command->set_new_layer_name(p_layer_name);
        change_layer_name_command->set_former_layer_name(current_map->get_layer_name(p_layer_id));

        commands.push_back(change_layer_name_command);

        commands::emitters::CommandToActionTransformer action_transformer;
        action_transformer.transform<node::IsometricMap, UndoRedo::MERGE_ALL>(
          commands,
          current_map,
          vformat(change_layer_name_action_name_format, p_layer_id)
        );
    }
}

void LayersEditor::_set_layer_visible(const uint32_t p_layer_id, CheckBox* p_check_box) { // NOLINT(*-convert-member-functions-to-static)
    Vector<Ref<commands::Command<node::IsometricMap>>> commands;

    Ref<commands::SetLayerVisibilityCommand> visibility_command;
    visibility_command.instantiate();
    visibility_command->set_layer_id(p_layer_id);
    visibility_command->set_visible(p_check_box->is_pressed());

    commands.push_back(visibility_command);

    commands::emitters::CommandToActionTransformer action_transformer;
    action_transformer.transform<node::IsometricMap, set_layer_visible_action_name>(
      commands,
      IsometricEditorPlugin::get_instance()->get_selected_map()
    );
}

void LayersEditor::_set_current_layer(uint32_t p_layer_id) { // NOLINT(*-convert-member-functions-to-static)
    if (node::IsometricMap* current_map{IsometricEditorPlugin::get_instance()->get_selected_map()}) {
        current_map->set_meta(node::IsometricMap::LAST_EDITED_LAYER_META_NAME,p_layer_id);
    }
}

void LayersEditor::_layer_color_changed(const Color& p_color, uint32_t p_layer_id) { // NOLINT(*-convert-member-functions-to-static)
    if (node::IsometricMap* map{IsometricEditorPlugin::get_instance()->get_selected_map()}) {
        map->set_layer_color(p_layer_id, p_color);
    }
}

void LayersEditor::_remove_layer(uint32_t p_layer_id, const String& p_layer_name) {
    if (node::IsometricMap* current_map = IsometricEditorPlugin::get_instance()->get_selected_map()) {
        Vector<Ref<commands::Command<node::IsometricMap>>> commands;

        Vector3 map_size {current_map->get_size()};
        for (int x = 0; x < static_cast<int>(map_size.x); ++x) {
            for (int y = 0; y < static_cast<int>(map_size.y); ++y) {
                for (int z = 0; z < static_cast<int>(map_size.z); ++z) {
                    Vector3i position_in_map {x, y, z};
                    uint32_t layer_id_at_position {current_map->get_layer_id_at(position_in_map)};

                    if (layer_id_at_position != p_layer_id) {
                        continue;
                    }

                    if (node::IsometricPositionable* positionable {current_map->get_positionable_at(position_in_map)}) {
                        Vector3 current_positionable_size {positionable->get_size()};

                        Ref<commands::AddPositionableCommand> add_command;
                        add_command.instantiate();
                        add_command->set_aabb({position_in_map, current_positionable_size});
                        add_command->set_positionable_id(current_map->get_positionable_id_for_position(position_in_map));
                        add_command->set_layer_id(layer_id_at_position);

                        Ref<commands::RevertCommand<node::IsometricMap>> delete_command;
                        delete_command.instantiate();
                        delete_command->set_reverse_command(add_command);

                        commands.push_back(delete_command);
                    }
                }
            }
        }

        Ref<commands::AddLayerCommand> add_layer_command;
        add_layer_command.instantiate();
        add_layer_command->set_layer_id(p_layer_id);
        add_layer_command->set_layer_name(p_layer_name);

        Ref<commands::RevertCommand<node::IsometricMap>> remove_layer_command;
        remove_layer_command.instantiate();
        remove_layer_command->set_reverse_command(add_layer_command);

        remove_layer_command->add_hook(callable_mp(this, &LayersEditor::refresh));

        commands.push_back(remove_layer_command);

        commands::emitters::CommandToActionTransformer action_transformer;
        action_transformer.transform<node::IsometricMap, remove_layer_action_name>(commands, current_map);
    }
}

#endif