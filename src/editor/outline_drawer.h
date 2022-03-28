#ifdef TOOLS_ENABLED

#ifndef ISOMETRIC_MAPS_OUTLINE_DRAWER_H
#define ISOMETRIC_MAPS_OUTLINE_DRAWER_H

#include <modules/isometric_maps/src/node/isometric_positionable.h>

namespace editor {
    class OutlineDrawer {
    public:
        static void
        draw_outline(node::IsometricPositionable* positionable, bool should_draw_polygon, const Color& color,
                     real_t line_size);

        static void set_outline_visible(node::IsometricPositionable* positionable, bool visible);
    };
} // namespace editor

#endif //ISOMETRIC_MAPS_OUTLINE_DRAWER_H

#endif