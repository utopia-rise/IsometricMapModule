#ifndef ISOMETRIC_MAPS_ISOMETRIC_ELEMENT_H
#define ISOMETRIC_MAPS_ISOMETRIC_ELEMENT_H

#include <core/math/aabb.h>
#include <core/math/transform_2d.h>
#include <core/rid.h>

namespace data {
    //forward declaration
    struct IsometricSpace;

    struct IsometricElement : public RID_Data {
        ////////Set by API calls (main thread)//////
        IsometricSpace* space = nullptr;
        bool is_dynamic = false;

        //////Set by commands (isometric server thread)///////
        AABB aabb  = AABB(Vector3(0., 0., 0.),Vector3( 1., 1., 1.));

        RID visual_rid = RID();
        int z_size = 1;
        int z_order = 0;

        LocalVector<IsometricElement*> behind_statics;
        LocalVector<IsometricElement*> behind_dynamics;

        bool dirty = true;
        bool in_stack = false;
        bool is_invalid = false;
    };
} // namespace data
#endif //ISOMETRIC_MAPS_ISOMETRIC_ELEMENT_H
