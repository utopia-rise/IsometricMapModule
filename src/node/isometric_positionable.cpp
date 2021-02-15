#include "isometric_positionable.h"
#include <modules/isometric_maps/src/isometric_server.h>

using namespace node;

IsometricPositionable::IsometricPositionable() :
        Node2D(), debug_points(), aabb({0, 0, 0}, {1, 1, 1}), z_order_size(0), debug_z(0), outline_drawer(nullptr),
        world(RID()), world_owner(false), self(get_canvas_item()) {
    on_resize();
}

void IsometricPositionable::_enter_tree() {
    set_z_order_size(1);

    if (world != RID()) {
        WARN_PRINT("unregister")
        IsometricServer::get_instance()->unregister_isometric_element(world, self);
    }
    world_owner = false;
    world = RID();

    const Node* parent{get_parent()};
    if (parent) {
        if (auto positionable{cast_to<IsometricPositionable>(parent)}) {
            if (positionable->world != RID()) {
                world = positionable->world;
                world_owner = false;
                aabb.position = local_position + positionable->aabb.position;
                IsometricServer::get_instance()->register_isometric_element(world, this->get_canvas_item(), is_dynamic);
                return;
            }
        }
    }
    world = IsometricServer::get_instance()->create_space();
    IsometricServer::get_instance()->register_isometric_element(world, this->get_canvas_item(), is_dynamic);
    world_owner = true;

}

void IsometricPositionable::_exit_tree() {
    if (self != RID()) {
        IsometricServer::get_instance()->unregister_isometric_element(world, self);
        if (world_owner) {
            world_owner = false;
        }
        world = RID();
    }
    if (outline_drawer) {
        memdelete(outline_drawer);
        outline_drawer = nullptr;
    }
}

void IsometricPositionable::set_outline_drawer(Color color, real_t line_size) {
    prepare_points();
    if (!outline_drawer) {
        outline_drawer = memnew(editor::OutlineDrawer());
        add_child(outline_drawer);
    }
    outline_drawer->setPoints(&up_points, &down_points);
    outline_drawer->setColor(color);
    outline_drawer->setLineSize(line_size);
    outline_drawer->update();
}

Vector3 IsometricPositionable::get_local_position_3d() const {
    return local_position;
}

void IsometricPositionable::set_local_position_3d(Vector3 p_local) {
    Vector3 offset = p_local - local_position;
    local_position = p_local;
    aabb.position += offset;
    //set_position(resource::IsometricConfiguration::getDefaultConfiguration()->from_3D_to_screen(p_local));
}

Vector3 IsometricPositionable::get_global_position_3d() const {
    return aabb.position;
}

void IsometricPositionable::set_global_position_3d(Vector3 pos) {
    Vector3 offset = pos - aabb.position;
    aabb.position = pos;
    local_position += offset;
    //set_position(resource::IsometricConfiguration::getDefaultConfiguration()->from_3D_to_screen(get_local_position_3d()));
}

AABB IsometricPositionable::get_aabb() {
    return aabb;
}

void IsometricPositionable::set_aabb(AABB ab) {
    Vector3 offset{ab.position - aabb.position};
    aabb = ab;
    local_position += offset;
    //set_position(resource::IsometricConfiguration::getDefaultConfiguration()->from_3D_to_screen(ab.position));
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

    Vector2 grid_slope_offset;

    int left_slope{0};
    int right_slope{0};
    int forward_slope{0};
    int backward_slope{0};


    const SlopeType &slope_type{
            calculate_slope_offset(&grid_slope_offset, tile_width_float, tile_height_float, w, d, ratio)};
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


    up_points.resize(0);
    up_points.push_back(points[4]);
    up_points.push_back(points[5]);
    up_points.push_back(points[6]);
    up_points.push_back(points[7]);

    left_points.resize(0);
    left_points.push_back(points[2]);
    left_points.push_back(points[3]);
    left_points.push_back(points[7]);
    left_points.push_back(points[6]);

    right_points.resize(0);
    right_points.push_back(points[1]);
    right_points.push_back(points[2]);
    right_points.push_back(points[6]);
    right_points.push_back(points[5]);

    down_points.resize(0);
    down_points.push_back(points[0]);
    down_points.push_back(points[1]);
    down_points.push_back(points[2]);
    down_points.push_back(points[3]);

    int deb_z{get_debug_z()};

    if (h > 0) {
        ratio = static_cast<real_t>(deb_z) / h;
    }

    if (deb_z > -1) {
        Vector2 gridOffset(0, -resource::IsometricConfiguration::getDefaultConfiguration()->get_e_z() *
                              static_cast<real_t>(deb_z));
        debug_points.resize(0);
        debug_points.push_back(points[0] + gridOffset + (right_slope + backward_slope) * grid_slope_offset);
        debug_points.push_back(points[1] + gridOffset + (left_slope + backward_slope) * grid_slope_offset);
        debug_points.push_back(points[2] + gridOffset + (left_slope + forward_slope) * grid_slope_offset);
        debug_points.push_back(points[3] + gridOffset + (right_slope + forward_slope) * grid_slope_offset);
    }
}

void IsometricPositionable::_notification(int notif) {
    switch (notif) {
        case NOTIFICATION_ENTER_TREE:
            _enter_tree();
            break;
        case NOTIFICATION_EXIT_TREE:
            _exit_tree();
            break;
        default:
            break;
    }
}

void IsometricPositionable::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_outline_drawer", "color", "line_size"),
                         &IsometricPositionable::set_outline_drawer);
    ClassDB::bind_method(D_METHOD("get_aabb"), &IsometricPositionable::get_aabb);
    ClassDB::bind_method(D_METHOD("set_aabb", "ab"), &IsometricPositionable::set_aabb);
    ClassDB::bind_method(D_METHOD("on_resize"), &IsometricPositionable::on_resize);
    ClassDB::bind_method(D_METHOD("on_grid_updated", "stair"), &IsometricPositionable::on_grid_updated);
    ClassDB::bind_method(D_METHOD("on_select", "selected"), &IsometricPositionable::on_select);

    ClassDB::bind_method(D_METHOD("set_global_position_3d", "pos"), &IsometricPositionable::set_global_position_3d);
    ClassDB::bind_method(D_METHOD("get_global_position_3d"), &IsometricPositionable::get_global_position_3d);
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "global_position_3d"), "set_global_position_3d",
                 "get_global_position_3d");

    ClassDB::bind_method(D_METHOD("set_local_position_3d", "p_local"), &IsometricPositionable::set_local_position_3d);
    ClassDB::bind_method(D_METHOD("get_local_position_3d"), &IsometricPositionable::get_local_position_3d);
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "local_position_3d"), "set_local_position_3d", "get_local_position_3d");

    ClassDB::bind_method(D_METHOD("set_has_moved", "hm"), &IsometricPositionable::set_has_moved);
    ClassDB::bind_method(D_METHOD("get_has_moved"), &IsometricPositionable::get_has_moved);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "has_moved"), "set_has_moved", "get_has_moved");

    ClassDB::bind_method(D_METHOD("set_size_3d", "s"), &IsometricPositionable::set_size_3d);
    ClassDB::bind_method(D_METHOD("get_size_3d"), &IsometricPositionable::get_size_3d);
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "size_3d"), "set_size_3d", "get_size_3d");
    ADD_PROPERTY_DEFAULT("size_3d", Vector3(1, 1, 1));

    ClassDB::bind_method(D_METHOD("set_z_order_size", "size"), &IsometricPositionable::set_z_order_size);
    ClassDB::bind_method(D_METHOD("get_z_order_size"), &IsometricPositionable::get_z_order_size);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "z_order_size"), "set_z_order_size", "get_z_order_size");
    ADD_PROPERTY_DEFAULT("z_order_size", 1);

    ClassDB::bind_method(D_METHOD("set_debug_z", "d_z"), &IsometricPositionable::set_debug_z);
    ClassDB::bind_method(D_METHOD("get_debug_z"), &IsometricPositionable::get_debug_z);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "debug_z"), "set_debug_z", "get_debug_z");
    ADD_PROPERTY_DEFAULT("debug_z", 0);

    //    BIND_ENUM_CONSTANT(NONE);
    //    BIND_ENUM_CONSTANT(LEFT);
    //    BIND_ENUM_CONSTANT(RIGHT);
    //    BIND_ENUM_CONSTANT(FORWARD);
    //    BIND_ENUM_CONSTANT(BACKWARD);
}
