#ifndef ISOMETRIC_MAPS_OUTLINE_DATA_H
#define ISOMETRIC_MAPS_OUTLINE_DATA_H

#ifdef TOOLS_ENABLED

    #include "core/color.h"
    #include "core/variant.h"
    #include <core/rid.h>

namespace editor {
    struct OutlineData {
        RID rid;
        bool is_visible;
        Color color;
        real_t line_size;
        bool should_draw_polygons;

        Vector<PoolVector2Array> debug_shape;

        OutlineData();
        ~OutlineData() = default;
    };
}// namespace editor

#endif// ISOMETRIC_MAPS_OUTLINE_DATA_H

#endif
