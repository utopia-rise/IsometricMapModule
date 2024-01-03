#ifdef TOOLS_ENABLED

#include "layers_editor.h"
#include "editor/isometric_editor_plugin.h"
#include "editor/commands/add_layer_command.h"
#include "editor/commands/revert_command.h"
#include "editor/commands/set_layer_visibility_command.h"

using namespace editor::inspector;

constexpr const char add_layer_action_name[] = "add_layer";
constexpr const char remove_layer_action_name[] = "remove_layer";
constexpr const char set_layer_visible_action_name[] = "set_layer_visible";

LayersEditor::LayersEditor() : layer_line_edit(nullptr), layer_controls_container(nullptr) {
    HBoxContainer* top_bar {memnew(HBoxContainer)};
    layer_line_edit = memnew(LineEdit);
    layer_line_edit->set_h_size_flags(SizeFlags::SIZE_EXPAND_FILL);
    Button* add_button {memnew(Button)};
    add_button->set_text("+");
    top_bar->add_child(layer_line_edit);
    top_bar->add_child(add_button);
    add_button->connect(SNAME("pressed"), Callable(this, "_add_layer"));
    add_child(top_bar);
    ScrollContainer* scroll_container {memnew(ScrollContainer)};
    layer_controls_container = memnew(GridContainer);
    layer_controls_container->set_columns(4);
    layer_controls_container->set_h_size_flags(SizeFlags::SIZE_EXPAND_FILL);
    scroll_container->add_child(layer_controls_container);
    add_child(scroll_container);
    refresh();

    scroll_container->set_h_size_flags(SizeFlags::SIZE_EXPAND_FILL);
    scroll_container->set_v_size_flags(SizeFlags::SIZE_EXPAND_FILL);
    set_h_size_flags(SizeFlags::SIZE_EXPAND_FILL);
    set_v_size_flags(SizeFlags::SIZE_EXPAND_FILL);

    current_layer_button_group = Ref<ButtonGroup>();
    current_layer_button_group.instantiate();
}

void LayersEditor::refresh() {
    TypedArray<Node> elements {layer_controls_container->get_children()};
    for (int i = 0; i < elements.size(); ++i) {
        Node* child = static_cast<Node*>(elements[i].operator Object*());
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
        Label* remove_label {memnew(Label)};
        remove_label->set_text("Remove");
        layer_controls_container->add_child(is_current_layer_label);
        layer_controls_container->add_child(layer_name_label);
        layer_controls_container->add_child(is_visible_label);
        layer_controls_container->add_child(remove_label);

        const Dictionary& layers = current_map->get_layers();
        Array ids = layers.keys();
        Array layer_names = layers.values();
        for (int i = 0; i < layers.size(); ++i) {
            const String& layer_name {layer_names[i]};
            const uint32_t layer_id {ids[i]};
            CurrentLayerCheckBox* current_layer_check_box {memnew(CurrentLayerCheckBox)};
            current_layer_check_box->set_layer_id(layer_id);
            current_layer_check_box->set_button_group(current_layer_button_group);
            layer_controls_container->add_child(current_layer_check_box);
            Label* current_layer_name_label {memnew(Label)};
            current_layer_name_label->set_text(layer_name);
            layer_controls_container->add_child(current_layer_name_label);
            LayerVisibleCheckBox* visible_check_box {memnew(LayerVisibleCheckBox)};
            visible_check_box->set_layer_id(layer_id);
            layer_controls_container->add_child(visible_check_box);
            LayerRemoveButton* layer_remove_button {memnew(LayerRemoveButton)};
            layer_remove_button->set_layer_informations(layer_id, layer_name);
            layer_remove_button->connect(SNAME("pressed"), Callable(this, "refresh"));
            layer_controls_container->add_child(layer_remove_button);
        }
    }
}

void LayersEditor::_add_layer() {
    Vector<Ref<commands::Command>> commands;
    Ref<commands::AddLayerCommand> add_layer_command;
    add_layer_command.instantiate();
    add_layer_command->set_layer_name(layer_line_edit->get_text());
    commands.push_back(add_layer_command);

    commands::emitters::CommandToActionTransformer action_transformer;
    action_transformer.transform<add_layer_action_name>(commands, IsometricEditorPlugin::get_instance()->get_selected_map());

    refresh();
}

void LayersEditor::_bind_methods() {
    ClassDB::bind_method(D_METHOD("_add_layer"), &LayersEditor::_add_layer);
    ClassDB::bind_method(D_METHOD("refresh"), &LayersEditor::refresh);
}

uint32_t LayersEditor::get_selected_layer_id() const {
    if (auto check_box {Object::cast_to<CurrentLayerCheckBox>(current_layer_button_group->get_pressed_button())}) {
        return check_box->get_layer_id();
    }

    return node::IsometricMap::DEFAULT_LAYER_ID;
}

///////////////////////////////// LayerVisibleCheckBox ////////////////////////////////////////////

void LayerVisibleCheckBox::set_layer_id(uint32_t p_layer_id) {
    layer_id = p_layer_id;
}

void LayerVisibleCheckBox::_set_layer_visible() {
    Vector<Ref<commands::Command>> commands;

    Ref<commands::SetLayerVisibilityCommand> visibility_command;
    visibility_command.instantiate();
    visibility_command->set_layer_id(layer_id);
    visibility_command->set_visible(is_pressed());

    commands.push_back(visibility_command);

    commands::emitters::CommandToActionTransformer action_transformer;
    action_transformer.transform<set_layer_visible_action_name>(
            commands,
            IsometricEditorPlugin::get_instance()->get_selected_map()
    );
}

void LayerVisibleCheckBox::_notification(int notif) {
    if (notif != NOTIFICATION_ENTER_TREE) {
        return;
    }

    set_pressed(true);

    connect(SNAME("pressed"), Callable(this, "_set_layer_visible"));
}

LayerVisibleCheckBox::LayerVisibleCheckBox() : layer_id(node::IsometricMap::NO_LAYER_ID) {

}

void LayerVisibleCheckBox::_bind_methods() {
    ClassDB::bind_method(D_METHOD("_set_layer_visible"), &LayerVisibleCheckBox::_set_layer_visible);
}

///////////////////////////////// LayerRemoveButton ////////////////////////////////////////////

void LayerRemoveButton::set_layer_informations(uint32_t p_layer_id, const String& p_layer_name) {
    layer_id = p_layer_id;
    layer_name = p_layer_name;

    connect(SNAME("pressed"), Callable(this, "_remove_layer"));

    set_text("-");
}

void LayerRemoveButton::_remove_layer() {
    if (node::IsometricMap* current_map = IsometricEditorPlugin::get_instance()->get_selected_map()) {
        Vector<Ref<commands::Command>> commands;

        Vector3 map_size {current_map->get_size()};
        for (int x = 0; x < static_cast<int>(map_size.x); ++x) {
            for (int y = 0; y < static_cast<int>(map_size.y); ++y) {
                for (int z = 0; z < static_cast<int>(map_size.z); ++z) {
                    Vector3i position {x, y, z};
                    uint32_t layer_id_at_position {current_map->get_layer_id_at(position)};

                    if (layer_id_at_position != layer_id) {
                        continue;
                    }

                    Vector3 current_positionable_size {current_map->get_positionable_at(position)->get_size()};

                    Ref<commands::AddPositionableCommand> add_command;
                    add_command.instantiate();
                    add_command->set_aabb({position, current_positionable_size});
                    add_command->set_positionable_id(current_map->get_positionable_id_for_position(position));
                    add_command->set_layer_id(layer_id_at_position);

                    Ref<commands::RevertCommand> delete_command;
                    delete_command.instantiate();
                    delete_command->set_reverse_command(add_command);

                    commands.push_back(delete_command);
                }
            }
        }

        Ref<commands::AddLayerCommand> add_layer_command;
        add_layer_command.instantiate();
        add_layer_command->set_layer_id(layer_id);
        add_layer_command->set_layer_name(layer_name);

        Ref<commands::RevertCommand> remove_layer_command;
        remove_layer_command.instantiate();
        remove_layer_command->set_reverse_command(add_layer_command);

        commands.push_back(remove_layer_command);

        commands::emitters::CommandToActionTransformer action_transformer;
        action_transformer.transform<remove_layer_action_name>(commands, current_map);
    }
}

LayerRemoveButton::LayerRemoveButton() : layer_id(node::IsometricMap::NO_LAYER_ID) {

}

void LayerRemoveButton::_bind_methods() {
    ClassDB::bind_method(D_METHOD("_remove_layer"), &LayerRemoveButton::_remove_layer);
}

///////////////////////////////// CurrentLayerCheckBox ////////////////////////////////////////////

uint32_t CurrentLayerCheckBox::get_layer_id() const {
    return layer_id;
}

void CurrentLayerCheckBox::set_layer_id(uint32_t p_layer_id) {
    layer_id = p_layer_id;
}

CurrentLayerCheckBox::CurrentLayerCheckBox() : layer_id(node::IsometricMap::NO_LAYER_ID) {

}

void CurrentLayerCheckBox::_bind_methods() {

}

#endif