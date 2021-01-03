#ifndef ISOMETRIC_MAPS_ISOMETRIC_TILE_H
#define ISOMETRIC_MAPS_ISOMETRIC_TILE_H


#include "static_isometric_element.h"

namespace positionable {

    class IsometricTile : public StaticIsometricElement{
        GDCLASS(IsometricTile, StaticIsometricElement)

    protected:
        static void _bind_methods();
    };
}


#endif //ISOMETRIC_MAPS_ISOMETRIC_TILE_H
