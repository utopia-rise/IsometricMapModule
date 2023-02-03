#ifdef TOOLS_ENABLED

#include "isometric_tile_editor_plugin.h"

#include "node/isometric_map.h"
#include "node/isometric_positionable.h"
#include "outline_drawer.h"

#include <editor/editor_node.h>

using namespace editor;

IsometricTileEditorPlugin* IsometricTileEditorPlugin::get_instance() {
    static IsometricTileEditorPlugin* instance {nullptr};
    if (unlikely(!instance && ObjectDB::instance_validate(EditorNode::get_undo_redo()))) {
        instance = memnew(IsometricTileEditorPlugin);
    }
    return instance;
}

bool IsometricTileEditorPlugin::handles(Object* p_object) const {
    return cast_to<node::IsometricPositionable>(p_object) && !cast_to<node::IsometricMap>(p_object);
}

void IsometricTileEditorPlugin::edit(Object* p_object) {
    selected_positionable = cast_to<node::IsometricPositionable>(p_object);
    OutlineData& outline_data {selected_positionable->get_outline_data()};
    outline_data.line_size = 1.0;
    outline_data.color = {0, 0, 0, 1};
    OutlineDrawer::draw_outline(selected_positionable);
    OutlineDrawer::set_outline_visible(selected_positionable, true);

    if (!selected_positionable->is_connected(node::IsometricPositionable::SIZE_CHANGED_SIGNAL, this, "_on_size_changed")) {
        selected_positionable->connect(node::IsometricPositionable::SIZE_CHANGED_SIGNAL, this, "_on_size_changed");
    }
}

void IsometricTileEditorPlugin::_on_size_changed() {
    OutlineDrawer::draw_outline(selected_positionable);
}

IsometricTileEditorPlugin::IsometricTileEditorPlugin() : selected_positionable{nullptr} {}

void IsometricTileEditorPlugin::_bind_methods() {
    ClassDB::bind_method("_on_size_changed", &IsometricTileEditorPlugin::_on_size_changed);
}

#endif