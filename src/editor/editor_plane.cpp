#ifdef TOOLS_ENABLED

#include "editor_plane.h"
#include <servers/visual_server.h>

using namespace editor;

EditorPlane::EditorPlane(int p_position, EditorAxes p_axes, const Vector2& p_size)
    : position(p_position),
    axes(p_axes),
    size(p_size),
    self(VisualServer::get_singleton()->canvas_item_create()){
}

int EditorPlane::get_position() const {
    return position;
}

void EditorPlane::set_position(int p_position) {
    position = p_position;
}

EditorAxes EditorPlane::get_axes() const {
    return axes;
}

const Vector2& EditorPlane::get_size() const {
    return size;
}

RID EditorPlane::get_rid() const {
    return self;
}

#endif
