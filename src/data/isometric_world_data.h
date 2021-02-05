#ifndef ISOMETRIC_MAPS_ISOMETRIC_WORLD_DATA_H
#define ISOMETRIC_MAPS_ISOMETRIC_WORLD_DATA_H

#include <core/rid.h>
#include <modules/isometric_maps/src/isometric_world.h>

class IsometricWorldData : public RID_Data {
public:
	IsometricWorld *world;

	IsometricWorldData();
	~IsometricWorldData() = default;
};


#endif //ISOMETRIC_MAPS_ISOMETRIC_WORLD_DATA_H
