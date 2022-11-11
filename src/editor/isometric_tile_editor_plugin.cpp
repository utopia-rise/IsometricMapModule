#ifdef TOOLS_ENABLED

    #include "isometric_tile_editor_plugin.h"
    #include "outline_drawer.h"
    #include <editor/editor_node.h>
    #include <modules/isometric_maps/src/node/isometric_map.h>
    #include <modules/isometric_maps/src/node/isometric_positionable.h>

using namespace editor;

IsometricTileEditorPlugin* IsometricTileEditorPlugin::get_instance() {
    static IsometricTileEditorPlugin* instance {nullptr};
    if (unlikely(!instance && ObjectDB::instance_validate(EditorNode::get_undo_redo()))) { instance = memnew(IsometricTileEditorPlugin); }
    return instance;
}

bool IsometricTileEditorPlugin::handles(Object* p_object) const {
    return cast_to<node::IsometricPositionable>(p_object) && !cast_to<node::IsometricMap>(p_object);
}

void IsometricTileEditorPlugin::edit(Object* p_object) {
    auto* positionable {cast_to<node::IsometricPositionable>(p_object)};
    OutlineData& outline_data {positionable->get_outline_data()};
    outline_data.line_size = 1.0;
    outline_data.color = {0, 0, 0, 1};
    OutlineDrawer::draw_outline(positionable);
    OutlineDrawer::set_outline_visible(positionable, true);
}

#endif