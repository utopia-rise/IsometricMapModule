#ifdef TOOLS_ENABLED

#include "positionable_set_inspector.h"
#include <scene/resources/packed_scene.h>
#include <modules/isometric_maps/src/node/isometric_map.h>
#include <scene/2d/camera_2d.h>
#include <editor/editor_node.h>
#include <modules/isometric_maps/src/editor/isometric_editor_plugin.h>

using namespace editor::inspector;

const char* SCENE_META{"positionable_scene"};

void PositionableSetInspector::update_for_positionable(Ref<PackedScene> positionable_scene) {
    StringName path{positionable_scene->get_path()};
    print_line(vformat("update for %s", path));

    if (buttons.has(path)) {
        print_line(vformat("already has %s", path));
        viewer->remove_child(buttons[path]);
    }
    InspectorButton* button{memnew(InspectorButton)};
    button->set_text(positionable_scene->get_path());
    Viewport* icon_viewport{_get_icon_for_scene(positionable_scene)};
    IsometricEditorPlugin::get_instance()->add_icon_viewport(icon_viewport);
    Ref<ViewportTexture> icon_texture{icon_viewport->get_texture()};
    button->set_icon(icon_texture);
    button->set_meta(SCENE_META, positionable_scene);
    viewer->add_child(button);
    buttons[path] = button;
    print_line(vformat("inserted button for path: %s", path));
}

void PositionableSetInspector::clear() {
    buttons.clear();
    for (int i = 0; i < viewer->get_child_count(); ++i) {
        if (auto* inspector_button{Object::cast_to<InspectorButton>(viewer->get_child(i))}) {
            print_line(vformat("Will clean button for index %s", i));
            viewer->remove_child(inspector_button);
            --i;
        }
    }
}

PositionableSetInspector::PositionableSetInspector(const String& title) : VBoxContainer(), title_label(memnew(Label)),
                                                                          scroll_container(memnew(ScrollContainer)),
                                                                          viewer(memnew(VBoxContainer)), buttons() {
    title_label->set_text(title);
    _init();
}

PositionableSetInspector::PositionableSetInspector() : VBoxContainer(), title_label(memnew(Label)),
                                                       scroll_container(memnew(ScrollContainer)),
                                                       viewer(memnew(VBoxContainer)), buttons() {
    _init();
}

void PositionableSetInspector::_init() {
    add_child(title_label);
    scroll_container->set_h_size_flags(SIZE_EXPAND_FILL);
    scroll_container->set_v_size_flags(SIZE_EXPAND_FILL);
    scroll_container->set_clip_contents(true);
    scroll_container->set_enable_h_scroll(false);
    scroll_container->set_enable_v_scroll(true);
    add_child(scroll_container);
    viewer->set_h_size_flags(SIZE_EXPAND_FILL);
    viewer->set_v_size_flags(SIZE_EXPAND_FILL);
    scroll_container->add_child(viewer);
}

void PositionableSetInspector::_bind_methods() {

}

Viewport* PositionableSetInspector::_get_icon_for_scene(Ref<PackedScene> scene) {
    if (auto* positionable{Object::cast_to<node::IsometricPositionable>(scene->instance())}) {
        const Vector2& original_scale{positionable->get_scale()};
        positionable->set_scale(original_scale * Vector2(1, -1));

        Viewport* viewport{memnew(Viewport)};
        Camera2D* camera{memnew(Camera2D)};
        viewport->add_child(camera);
        viewport->add_child(positionable);
        viewport->set_update_mode(Viewport::UPDATE_ONCE);
//        Vector2 size{
//            static_cast<float>(IsometricServer::get_instance()->get_isometric_space_diamond_width(positionable->get_space_RID())),
//            static_cast<float>(IsometricServer::get_instance()->get_isometric_space_diamond_height(positionable->get_space_RID()))
//        };
        viewport->set_size({100, 100});
        EditorNode::get_singleton()->add_child(viewport);
        return viewport;
    }
    return nullptr;
}

#endif
