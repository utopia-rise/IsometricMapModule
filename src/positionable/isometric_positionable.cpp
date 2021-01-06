#include "isometric_positionable.h"
#include <modules/isometric_maps/src/isometric_world.h>
#include <modules/isometric_maps/src/isometric_api.h>

using namespace positionable;

IsometricPositionable::IsometricPositionable() : debug_points(), aabb({0, 0, 0}, {1, 1, 1}),
                                                 z_order_size(0), rendered(false), is_temporary(true), debug_z(0),
                                                 outline_drawer(nullptr), world(nullptr), world_owner(false) {
    connect("enter_tree", this, "_enter_tree");
    connect("exit_tree", this, "_exit_tree");
    on_resize();
}

void IsometricPositionable::_enter_tree() {
    set_z_order_size(1);

    if (world && world_owner) {
        delete world;
        world_owner = false;
    }
    world = nullptr;

    const Node *parent{get_parent()};
    if (parent) {
        if (auto positionable{cast_to<IsometricPositionable>(parent)}) {
            if (positionable->world && !is_temporary) {
                world = positionable->world;
                world_owner = false;
                aabb.position = local_position + positionable->aabb.position;
                world->register_isometric_element(this);
                return;
            }
        }
    }
    world = new IsometricWorld();
    world_owner = true;
}

void IsometricPositionable::_exit_tree() {
    if (world) {
        world->unregister_isometric_element(this);
        if (world_owner) {
            delete world;
            world_owner = false;
        }
        world = nullptr;
    }
    if (outline_drawer) {
        memdelete(outline_drawer);
        outline_drawer = nullptr;
    }
}

Transform2D IsometricPositionable::get_hexagone_coordinates() const {
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

void IsometricPositionable::set_outline_drawer(Color color, real_t lineSize) {
    prepare_points();
    if (!outline_drawer) {
        outline_drawer = memnew(editor::OutlineDrawer());
        add_child(outline_drawer);
    }
    outline_drawer->setPoints(&up_points, &down_points);
    outline_drawer->setColor(color);
    outline_drawer->setLineSize(lineSize);
    outline_drawer->update();
}

Vector3 IsometricPositionable::get_local_position_3d() const {
    return local_position;
}

void IsometricPositionable::set_local_position_3d(Vector3 p_local) {
    Vector3 offset = p_local - local_position;
    local_position = p_local;
    aabb.position += offset;
    set_position(IsometricApi::get_instance()->get_screen_coord_from_3d(p_local));
}

Vector3 IsometricPositionable::get_global_position_3d() const {
    return aabb.position;
}

void IsometricPositionable::set_global_position_3d(Vector3 pos) {
    Vector3 offset = pos - aabb.position;
    aabb.position = pos;
    local_position += offset;
    set_position(IsometricApi::get_instance()->get_screen_coord_from_3d(get_local_position_3d()));
}

AABB IsometricPositionable::get_aabb() {
    return aabb;
}

void IsometricPositionable::set_aabb(AABB ab) {
    Vector3 offset {ab.position - aabb.position};
    aabb = ab;
    local_position += offset;
    set_position(IsometricApi::get_instance()->get_screen_coord_from_3d(ab.position));
    on_resize();
}

Vector3 IsometricPositionable::get_size_3d() const {
    return aabb.size;
}

void IsometricPositionable::set_size_3d(Vector3 s) {
    aabb.size = s;
    on_resize();
}

int IsometricPositionable::get_z_order_size() const {
    return z_order_size;
}

void IsometricPositionable::set_z_order_size(int size) {
    z_order_size = size;
}

bool IsometricPositionable::is_rendered() const {
    return rendered;
}

void IsometricPositionable::set_rendered(bool is_rendered) {
    rendered = is_rendered;
}

bool IsometricPositionable::get_is_temporary() const {
    return is_temporary;
}

void IsometricPositionable::set_is_temporary(bool temp) {
    is_temporary = temp;
    update();
}

int IsometricPositionable::get_debug_z() const {
    return debug_z;
}

void IsometricPositionable::set_debug_z(int d_z) {
    debug_z = d_z;
}

void IsometricPositionable::on_resize() {
    if (outline_drawer) {
        set_outline_drawer(outline_drawer->get_color(), outline_drawer->get_line_size());
    }
}

void IsometricPositionable::on_grid_updated(int stair) {

}

void IsometricPositionable::on_select(bool selected) {
    if (selected) {
        set_outline_drawer(Color(255, 0, 0, 1), 10.0f);
    } else if (outline_drawer) {
        remove_child(outline_drawer);
        memdelete(outline_drawer);
        outline_drawer = nullptr;
    }
}

bool IsometricPositionable::get_has_moved() const {
    return false;
}

void IsometricPositionable::set_has_moved(bool hm) {

}

void IsometricPositionable::prepare_points() {
    const Vector3 &size{get_size_3d()};
    real_t w{size.x};
    real_t d{size.y};
    real_t h{size.z};

    int left_slope{0};
    int right_slope{0};
    int forward_slope{0};
    int backward_slope{0};

    int tile_width{IsometricApi::get_instance()->get_tile_width()};
    int tile_height{IsometricApi::get_instance()->get_tile_height()};

    Vector2 offset(0, static_cast<real_t>(-tile_height) * 0.5f);

    float ratio{0};

    int deb_z{get_debug_z()};

    if (h > 0) {
        ratio = static_cast<real_t>(deb_z) / h;
    }

    auto tile_width_float = static_cast<real_t>(tile_width);
    auto tile_height_float = static_cast<real_t>(tile_height);
    Vector2 grid_slope_offset;

    const SlopeType &slope_type{calculate_slope_offset(&grid_slope_offset, tile_width_float, tile_height_float, w, d, ratio)};
    switch (slope_type) {
        case SlopeType::NONE:
            break;
        case SlopeType::LEFT:
            left_slope = 1;
            break;
        case SlopeType::RIGHT:
            right_slope = 1;
            break;
        case SlopeType::FORWARD:
            forward_slope = 1;
            break;
        case SlopeType::BACKWARD:
            backward_slope = 1;
            break;
    }

    PoolVector2Array points;

    //Lower points
    points.push_back(Vector2(0, 0));
    points.push_back(Vector2(tile_width_float * 0.5f * w, tile_height_float * 0.5f * w));
    points.push_back(Vector2(tile_width_float * 0.5f * (w - d), tile_height_float * 0.5f * (d + w)));
    points.push_back(Vector2(-tile_width_float * 0.5f * d, tile_height_float * 0.5f * d));

    Vector2 heightOffset(0, -IsometricApi::get_instance()->get_e_z() * h);

    //Upper points
    points.push_back(points[0] + (1 - (right_slope + backward_slope)) * heightOffset);
    points.push_back(points[1] + (1 - (left_slope + backward_slope)) * heightOffset);
    points.push_back(points[2] + (1 - (left_slope + forward_slope)) * heightOffset);
    points.push_back(points[3] + (1 - (right_slope + forward_slope)) * heightOffset);

    up_points.resize(0);
    up_points.push_back(offset + points[4]);
    up_points.push_back(offset + points[5]);
    up_points.push_back(offset + points[6]);
    up_points.push_back(offset + points[7]);

    left_points.resize(0);
    left_points.push_back(offset + points[2]);
    left_points.push_back(offset + points[3]);
    left_points.push_back(offset + points[7]);
    left_points.push_back(offset + points[6]);

    right_points.resize(0);
    right_points.push_back(offset + points[1]);
    right_points.push_back(offset + points[2]);
    right_points.push_back(offset + points[6]);
    right_points.push_back(offset + points[5]);

    down_points.resize(0);
    down_points.push_back(offset + points[0]);
    down_points.push_back(offset + points[1]);
    down_points.push_back(offset + points[2]);
    down_points.push_back(offset + points[3]);

    if (deb_z > -1) {
        Vector2 gridOffset(0, -IsometricApi::get_instance()->get_e_z() * static_cast<real_t>(deb_z));
        debug_points.resize(0);
        debug_points.push_back(offset + points[0] + gridOffset + (right_slope + backward_slope) * grid_slope_offset);
        debug_points.push_back(offset + points[1] + gridOffset + (left_slope + backward_slope) * grid_slope_offset);
        debug_points.push_back(offset + points[2] + gridOffset + (left_slope + forward_slope) * grid_slope_offset);
        debug_points.push_back(offset + points[3] + gridOffset + (right_slope + forward_slope) * grid_slope_offset);
    }
}

IsometricPositionable::SlopeType
IsometricPositionable::calculate_slope_offset(Vector2* slopeOffset, real_t tileWidthFloat,
                                              real_t tileHeightFloat, real_t width, real_t depth,
                                              real_t ratio) const {
    return SlopeType::NONE;
}

void IsometricPositionable::_bind_methods() {
    ClassDB::bind_method(D_METHOD("_enter_tree"), &IsometricPositionable::_enter_tree);
    ClassDB::bind_method(D_METHOD("_exit_tree"), &IsometricPositionable::_exit_tree);

    ClassDB::bind_method(D_METHOD("get_hexagone_coordinates"), &IsometricPositionable::get_hexagone_coordinates);
    ClassDB::bind_method(D_METHOD("set_outline_drawer"), &IsometricPositionable::set_outline_drawer);
    ClassDB::bind_method(D_METHOD("get_aabb"), &IsometricPositionable::get_aabb);
    ClassDB::bind_method(D_METHOD("set_aabb"), &IsometricPositionable::set_aabb);
    ClassDB::bind_method(D_METHOD("on_resize"), &IsometricPositionable::on_resize);
    ClassDB::bind_method(D_METHOD("on_grid_updated"), &IsometricPositionable::on_grid_updated);
    ClassDB::bind_method(D_METHOD("on_select"), &IsometricPositionable::on_select);
    ClassDB::bind_method(D_METHOD("set_has_moved"), &IsometricPositionable::set_has_moved);

    ClassDB::bind_method(D_METHOD("set_global_position_3d"), &IsometricPositionable::set_global_position_3d);
    ClassDB::bind_method(D_METHOD("get_global_position_3d"), &IsometricPositionable::get_global_position_3d);
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "global_position_3d"), "set_global_position_3d", "get_global_position_3d");

    ClassDB::bind_method(D_METHOD("set_local_position_3d"), &IsometricPositionable::set_local_position_3d);
    ClassDB::bind_method(D_METHOD("get_local_position_3d"), &IsometricPositionable::get_local_position_3d);
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "local_position_3d"), "set_local_position_3d", "get_local_position_3d");

    ClassDB::bind_method(D_METHOD("set_has_moved"), &IsometricPositionable::set_has_moved);
    ClassDB::bind_method(D_METHOD("get_has_moved"), &IsometricPositionable::get_has_moved);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "has_moved"), "set_has_moved", "get_has_moved");

    ClassDB::bind_method(D_METHOD("set_size_3d"), &IsometricPositionable::set_size_3d);
    ClassDB::bind_method(D_METHOD("get_size_3d"), &IsometricPositionable::get_size_3d);
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "size_3d"), "set_size_3d", "get_size_3d");
    ADD_PROPERTY_DEFAULT("size_3d", Vector3(1, 1, 1));

    ClassDB::bind_method(D_METHOD("set_z_order_size"), &IsometricPositionable::set_z_order_size);
    ClassDB::bind_method(D_METHOD("get_z_order_size"), &IsometricPositionable::get_z_order_size);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "z_order_size"), "set_z_order_size", "get_z_order_size");
    ADD_PROPERTY_DEFAULT("z_order_size", 1);

    ClassDB::bind_method(D_METHOD("set_is_temporary"), &IsometricPositionable::set_is_temporary);
    ClassDB::bind_method(D_METHOD("get_is_temporary"), &IsometricPositionable::get_is_temporary);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_temporary"), "set_is_temporary", "get_is_temporary");
    ADD_PROPERTY_DEFAULT("is_temporary", true);

    ClassDB::bind_method(D_METHOD("set_debug_z"), &IsometricPositionable::set_debug_z);
    ClassDB::bind_method(D_METHOD("get_debug_z"), &IsometricPositionable::get_debug_z);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "debug_z"), "set_debug_z", "get_debug_z");
    ADD_PROPERTY_DEFAULT("debug_z", 0);

//    BIND_ENUM_CONSTANT(NONE);
//    BIND_ENUM_CONSTANT(LEFT);
//    BIND_ENUM_CONSTANT(RIGHT);
//    BIND_ENUM_CONSTANT(FORWARD);
//    BIND_ENUM_CONSTANT(BACKWARD);
}