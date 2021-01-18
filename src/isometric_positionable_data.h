#ifndef ISOMETRIC_MAPS_ISOMETRIC_POSITIONABLE_DATA_H
#define ISOMETRIC_MAPS_ISOMETRIC_POSITIONABLE_DATA_H


#include <core/rid.h>
#include <core/math/aabb.h>
#include <core/math/transform_2d.h>

class IsometricPositionableData {
public:
    RID rid;
    AABB aabb;

    Vector<IsometricPositionableData*> behind_statics;
    Vector<IsometricPositionableData*> behind_dynamics;

    Transform2D get_hexagone_coordinates() const;
};


#endif //ISOMETRIC_MAPS_ISOMETRIC_POSITIONABLE_DATA_H
