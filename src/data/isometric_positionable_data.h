#ifndef ISOMETRIC_MAPS_ISOMETRIC_POSITIONABLE_DATA_H
#define ISOMETRIC_MAPS_ISOMETRIC_POSITIONABLE_DATA_H

#include <core/math/aabb.h>
#include <core/math/transform_2d.h>
#include <core/rid.h>

namespace data {
class IsometricPositionableData : public RID_Data {
public:
	RID visual_rid;
	AABB aabb;

	bool is_dynamic;

	int z_order;
	bool dirty;
	int z_size;

	Vector<IsometricPositionableData *> behind_statics;
	Vector<IsometricPositionableData *> behind_dynamics;

	IsometricPositionableData();
	~IsometricPositionableData() = default;
};
} // namespace data
#endif //ISOMETRIC_MAPS_ISOMETRIC_POSITIONABLE_DATA_H
