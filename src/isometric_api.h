#ifndef ISOMETRIC_MAPS_ISOMETRIC_API_H
#define ISOMETRIC_MAPS_ISOMETRIC_API_H


#include <core/object.h>
#include <modules/isometric_maps/src/positionable/isometric_map.h>

#define ISO_GROUP "positionables"

class IsometricApi : public Object {
    GDCLASS(IsometricApi, Object)
private:
    int tile_width;
    int tile_height;
    int angle;
    float e_z;
    float z_ratio;
    float topological_margin;

    ~IsometricApi() override = default;

public:
    IsometricApi();
    IsometricApi(const IsometricApi&) = delete;

    static IsometricApi* get_instance();

private:
    int calculate_tile_height() const;
    float calculate_ez() const;

public:
    int get_tile_width() const;
    void set_tile_width(int t_w);
    int get_tile_height() const;
    int get_angle() const;
    void set_angle(int agl);

    float get_e_z() const;
    float get_z_ratio() const;

    float get_topological_margin() const;
    void set_topological_margin(float margin);

    Vector2 get_screen_coord_from_3d(const Vector3& pos) const;
    Vector3 get_3d_coord_from_screen(const Vector2& pos, real_t orth_z) const;
    bool do_hexagone_overlap(const Transform2D& hex1, const Transform2D& hex2);
    bool is_box_in_front(const AABB& box, const AABB& other);

protected:
    static void _bind_methods();
};


#endif //ISOMETRIC_MAPS_ISOMETRIC_API_H
