#ifndef ISOMETRIC_MAPS_ISOMETRIC_TILE_H
#define ISOMETRIC_MAPS_ISOMETRIC_TILE_H

#include "isometric_element.h"
namespace positionable {

class IsometricTile : public IsometricElement {
	GDCLASS(IsometricTile, IsometricElement)

protected:
	static void _bind_methods();
};
} // namespace positionable

#endif //ISOMETRIC_MAPS_ISOMETRIC_TILE_H
