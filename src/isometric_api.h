#ifndef ISOMETRIC_MAPS_ISOMETRIC_API_H
#define ISOMETRIC_MAPS_ISOMETRIC_API_H


#include <core/object.h>
#include <modules/isometric_maps/src/positionable/isometric_map.h>

class IsometricApi : public Object {
private:
    IsometricApi();
    ~IsometricApi() override = default;

public:
    IsometricApi(const IsometricApi&) = delete;
    IsometricApi& operator=(const IsometricApi&) = delete;

    static IsometricApi* get_instance();

private:
    int calculate_tile_height() const;
    float calculate_ez() const;

public:
    Vector2 get_screen_coord_from_3d(positionable::IsometricMap* map, const Vector3& pos) const;
    Vector3 get_3d_coord_from_screen(positionable::IsometricMap* map, const Vector2& pos, real_t orthZ) const;
    bool do_hexagone_overlap(positionable::IsometricMap* map, const Transform2D& hex1, const Transform2D& hex2);
    bool is_box_in_front(positionable::IsometricMap* map, const AABB& box, const AABB& other);

protected:
    static void _bind_methods();
};


#endif //ISOMETRIC_MAPS_ISOMETRIC_API_H
