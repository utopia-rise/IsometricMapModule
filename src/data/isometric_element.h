#ifndef ISOMETRIC_MAPS_ISOMETRIC_ELEMENT_H
#define ISOMETRIC_MAPS_ISOMETRIC_ELEMENT_H

#include <core/math/aabb.h>
#include <core/math/transform_2d.h>
#include <core/rid.h>

namespace data {
    struct IsometricElement : public RID_Data {
        RID self;
        RID world;

        RID visual_rid;
        AABB aabb;

        bool is_dynamic;

        int z_order;
        bool dirty;
        int z_size;

        Vector<IsometricElement*> behind_statics;
        Vector<IsometricElement*> behind_dynamics;

        IsometricElement();
    };
} // namespace data
#endif //ISOMETRIC_MAPS_ISOMETRIC_ELEMENT_H
