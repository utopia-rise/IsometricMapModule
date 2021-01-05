#include "isometric_api.h"

IsometricApi::IsometricApi() : tile_width(256), angle(30), topological_margin(0.0f){
    tile_height = calculate_tile_height();
    e_z = calculate_ez();
    z_ratio = e_z / static_cast<float>(tile_height);
}

IsometricApi* IsometricApi::get_instance() {
    static IsometricApi singleton;
    return &singleton;
}

float IsometricApi::get_topological_margin() const {
    return topological_margin;
}

void IsometricApi::set_topological_margin(float margin) {
    topological_margin = margin;
}

Vector2 IsometricApi::get_screen_coord_from_3d(const Vector3& pos) const {
    return {
            (pos.x - pos.y) * static_cast<real_t>(tile_width) * 0.5f,
            (pos.x + pos.y) * static_cast<real_t>(tile_height) * 0.5f - e_z * pos.z
    };
}

Vector3 IsometricApi::get_3d_coord_from_screen(const Vector2& pos, real_t orth_z) const {
    real_t iso_x{pos.x};
    real_t iso_y{pos.y};
    auto tile_width_float = static_cast<real_t>(tile_width);
    auto tile_height_float = static_cast<real_t>(tile_height);
    real_t orth_x{iso_x / tile_width_float + (iso_y + orth_z * e_z) / tile_height_float};
    real_t orth_y{(iso_y + orth_z * e_z) / tile_height_float - iso_x / tile_width_float};
    return {
            ::roundf(orth_x),
            ::roundf(orth_y),
            ::roundf(orth_z)
    };
}

bool IsometricApi::do_hexagone_overlap(const Transform2D& hex1, const Transform2D& hex2) {
    return !(hex1.get_axis(0).x >= hex2.get_axis(0).y || hex2.get_axis(0).x >= hex1.get_axis(0).y)
           && !(hex1.get_axis(1).x >= hex2.get_axis(1).y || hex2.get_axis(1).x >= hex1.get_axis(1).y)
           && !(hex1.get_origin().x >= hex2.get_origin().y || hex2.get_origin().x >= hex1.get_origin().y);
}

bool IsometricApi::is_box_in_front(const AABB& box, const AABB& other) {
    const Vector3 &boxEnd{box.position + box.size};
    const Vector3 &otherEnd{other.position + other.size};

    if (boxEnd.x - other.position.x <= topological_margin) {
        return false;
    } else if (otherEnd.x - box.position.x <= topological_margin) {
        return true;
    }

    if (boxEnd.y - other.position.y <= topological_margin) {
        return false;
    } else if (otherEnd.y - box.position.y <= topological_margin) {
        return true;
    }

    if (boxEnd.z - other.position.z <= topological_margin) {
        return false;
    } else if (otherEnd.z - box.position.z <= topological_margin) {
        return true;
    }

    const Vector3 &distance{box.position + boxEnd - other.position - otherEnd};
    const Vector3 &cameraVector{1, 1, z_ratio};
    return distance.dot(cameraVector) >= 0;
}

int IsometricApi::calculate_tile_height() const {
    return static_cast<int>(round(tile_width * sin(Math::deg2rad(static_cast<double>(angle)))));
}

float IsometricApi::calculate_ez() const {
    return static_cast<float>((tile_width / sqrt(2)) * cos(Math::deg2rad(static_cast<double>(angle))));
}

void IsometricApi::_bind_methods() {

}

int IsometricApi::get_tile_width() const {
    return tile_width;
}

void IsometricApi::set_tile_width(int t_w) {
    tile_width = t_w;
    tile_height = calculate_tile_height();
    e_z = calculate_ez();
}

int IsometricApi::get_tile_height() const {
    return tile_height;
}

int IsometricApi::get_angle() const {
    return angle;
}

void IsometricApi::set_angle(int agl) {
    angle = agl;
    tile_height = calculate_tile_height();
    e_z = calculate_ez();
}

float IsometricApi::get_e_z() const {
    return e_z;
}

float IsometricApi::get_z_ratio() const {
    return z_ratio;
}
