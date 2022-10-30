#ifndef ISOMETRIC_MAPS_ISOMETRIC_ELEMENT_H
#define ISOMETRIC_MAPS_ISOMETRIC_ELEMENT_H

#include <core/math/aabb.h>
#include <core/math/transform_2d.h>
#include <core/rid.h>

namespace data {
    struct IsometricElement : public RID_Data {
        RID self = RID();
        RID world = RID();

        RID visual_rid = RID();
        AABB aabb  = AABB(Vector3(0., 0., 0.),Vector3( 1., 1., 1.));

        bool is_dynamic = false;

        int z_order = 0;
        int z_order_update = 0;
        bool dirty = true;
        bool to_delete = false;
        int z_size = 1;

        Vector<IsometricElement*> behind_statics;
        Vector<IsometricElement*> behind_dynamics;

        bool is_invalid = false;
    };
} // namespace data
#endif //ISOMETRIC_MAPS_ISOMETRIC_ELEMENT_H
