#ifdef TOOLS_ENABLED

#include "editor_plane.h"

using namespace editor;

EditorPlane::EditorPlane(int p_position, EditorAxes p_axes, const Vector2& p_size)
    : position(p_position), axes(p_axes), size(p_size) {

}

int EditorPlane::get_position() const {
    return position;
}

EditorAxes EditorPlane::get_axes() const {
    return axes;
}

const Vector2& EditorPlane::get_size() const {
    return size;
}

#endif
