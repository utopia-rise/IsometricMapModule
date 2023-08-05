#ifndef ISOMETRIC_MAPS_OUTLINE_DATA_H
#define ISOMETRIC_MAPS_OUTLINE_DATA_H

#include "core/math/color.h"
#include "core/templates/rid.h"
#include "core/variant/variant.h"

#ifdef TOOLS_ENABLED

namespace editor {
    struct OutlineData {
        RID rid;
        bool is_visible;
        Color color;
        real_t line_size;
        bool should_draw_polygons;

        Vector<PackedVector2Array> debug_shape;

        OutlineData();
        ~OutlineData() = default;
    };
}// namespace editor

#endif

#endif// ISOMETRIC_MAPS_OUTLINE_DATA_H


