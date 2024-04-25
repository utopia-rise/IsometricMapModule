#ifndef ISOMETRIC_MAPS_ISOMETRIC_ELEMENT_H
#define ISOMETRIC_MAPS_ISOMETRIC_ELEMENT_H

#include <core/math/aabb.h>
#include <core/math/transform_2d.h>
#include <core/templates/rid.h>

namespace data {
    // forward declaration
    struct IsometricSpace;

    struct IsometricElement {
        ////////Set by API calls (main thread)//////
        IsometricSpace* space = nullptr;
        RID visual_rid = RID();
        int depth = 1;
        bool is_dynamic = false;
        bool dirty = true;
        bool in_stack = false;
        bool is_invalid = false;

        LocalVector<IsometricElement*> behind_statics;
        LocalVector<IsometricElement*> behind_dynamics;

        //////Set by commands (isometric server thread)///////
        AABB aabb = AABB(Vector3(0., 0., 0.), Vector3(1., 1., 1.));

        int z_order = 0;

#ifdef TOOLS_ENABLED
        Color editor_modulate = Color(1, 1, 1);
#endif
    };
}// namespace data
#endif// ISOMETRIC_MAPS_ISOMETRIC_ELEMENT_H
