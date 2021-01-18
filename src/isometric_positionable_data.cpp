#include "isometric_positionable_data.h"
#include "isometric_api.h"

Transform2D IsometricPositionableData::get_hexagone_coordinates() const {
    const Vector3 &ortho_position{aabb.position};
    const Vector3 &size{aabb.size};
    const Vector3 &upper_point{Vector3(ortho_position.x, ortho_position.y,
                                       ortho_position.z + IsometricApi::get_instance()->get_z_ratio() * size.z)};
    const Vector3 &lower_point{Vector3(ortho_position.x + size.x, ortho_position.y + size.y, ortho_position.z)};
    const Vector3 &left_point{Vector3(ortho_position.x, ortho_position.y + size.y, ortho_position.z)};
    const Vector3 &right_point{Vector3(ortho_position.x + size.x, ortho_position.y, ortho_position.z)};
    real_t minX = upper_point.x - upper_point.z;
    real_t maxX = lower_point.x - lower_point.z;
    real_t minY = upper_point.y - upper_point.z;
    real_t maxY = lower_point.y - lower_point.z;
    real_t hMin = left_point.x - left_point.y;
    real_t hMax = right_point.x - right_point.y;
    return {minX, maxX, minY, maxY, hMin, hMax};
}
