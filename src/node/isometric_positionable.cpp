#include "isometric_positionable.h"
#include <modules/isometric_maps/src/isometric_server.h>
#include <modules/isometric_maps/src/utils/isometric_maths.h>

using namespace node;

IsometricPositionable::IsometricPositionable() :
        Node2D(), size({1, 1, 1}), z_order_size(0), is_dynamic(false), outline_drawer(nullptr),
        world(RID()), world_owner(false), self(RID()) {
}

void IsometricPositionable::_enter_tree() {
    set_z_order_size(1);

    if (world != RID()) {
        IsometricServer::get_instance()->unregister_isometric_element(world, self);
    }

    world_owner = false;
    world = RID();

    if (const Node* parent{get_parent()}) {
        if (auto positionable{cast_to<IsometricPositionable>(parent)}) {
            if (positionable->world != RID()) {

                world = positionable->world;
                world_owner = false;
                self = IsometricServer::get_instance()->register_isometric_element(world, this->get_canvas_item(),
                                                                                   is_dynamic);
                update_position();
                return;
            }
        }
    }

    world = IsometricServer::get_instance()->create_space();
    world_owner = true;


    self = IsometricServer::get_instance()->register_isometric_element(world, this->get_canvas_item(),
                                                                       is_dynamic);
    update_position();
}

void IsometricPositionable::_process() {
    set_z_order_size(IsometricServer::get_instance()->get_isometric_z_index(self));
}

void IsometricPositionable::_exit_tree() {
    if (self != RID()) {
        IsometricServer::get_instance()->unregister_isometric_element(world, self);
        if (world_owner) {
            world_owner = false;
            IsometricServer::get_instance()->delete_space(world);
        }
        world = RID();
    }
    if (outline_drawer) {
        memdelete(outline_drawer);
        outline_drawer = nullptr;
    }
}

void IsometricPositionable::update_position() {
    const data::IsometricParameters* params = IsometricServer::get_instance()->get_space_configuration(world);

    Vector2 position2D = utils::from_3D_to_screen(*params, local_position);
    Transform2D transform = get_transform();
    transform.set_origin(position2D);

    set_transform(transform);

    set_outline_drawer(Color(1., 0., 0.), 3);
}

void IsometricPositionable::set_outline_drawer(Color color, real_t line_size) {
    prepare_points();
    if (!outline_drawer) {
        outline_drawer = memnew(editor::OutlineDrawer());
        outline_drawer->set_visible(false);
        add_child(outline_drawer);
    }
    update();

    outline_drawer->setPoints(&up_points, &down_points);
    outline_drawer->setColor(color);
    outline_drawer->setLineSize(line_size);
    outline_drawer->update();
}

Vector3 IsometricPositionable::get_local_position_3d() const {
    return local_position;
}

void IsometricPositionable::set_local_position_3d(Vector3 p_local) {
    local_position = p_local;

    IsometricServer::get_instance()->set_isometric_element_position(self, get_global_position_3d());

    update_position();
}

Vector3 IsometricPositionable::get_global_position_3d() const {
    Vector3 global_position = local_position;

    IsometricPositionable* parent = Object::cast_to<IsometricPositionable>(get_parent());
    if(parent){
        global_position += parent->get_global_position_3d();
    }
    return global_position;
}

Vector3 IsometricPositionable::get_size() const {
    return size;
}

void IsometricPositionable::set_size(Vector3 s) {
    size = s;
    IsometricServer::get_instance()->set_isometric_element_size(self, size);
    set_outline_drawer(Color(1., 0., 0.), 3);
}

int IsometricPositionable::get_z_order_size() const {
    return z_order_size;
}

void IsometricPositionable::set_z_order_size(int size) {
    z_order_size = size;
}


void IsometricPositionable::prepare_points() {

    Vector2 grid_slope_offset;

    int no_slope{0};
    int left_slope{0};
    int right_slope{0};
    int forward_slope{0};
    int backward_slope{0};

    switch (slope_type) {
        case SlopeType::NONE:
            no_slope = 1;
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

    PoolVector2Array points{
            utils::get_bounding_box(
                    *IsometricServer::get_instance()->get_space_configuration(world),
                    size)};

    up_points.resize(0);
    up_points.push_back((no_slope + left_slope + forward_slope) * points[4] + (right_slope + backward_slope) * points[0]);
    up_points.push_back((no_slope + right_slope + forward_slope) * points[5] + (left_slope + backward_slope) * points[1]);
    up_points.push_back((no_slope + right_slope + backward_slope) * points[6] + (left_slope + forward_slope) * points[2]);
    up_points.push_back((no_slope + left_slope + backward_slope) * points[7] + (right_slope + forward_slope) * points[3]);

    down_points.resize(0);
    down_points.push_back(points[0]);
    down_points.push_back(points[1]);
    down_points.push_back(points[2]);
    down_points.push_back(points[3]);
}

void IsometricPositionable::_notification(int notif) {
    switch (notif) {
        case NOTIFICATION_ENTER_TREE:
            _enter_tree();
            break;
        case NOTIFICATION_EXIT_TREE:
            _exit_tree();
            break;
        case NOTIFICATION_PROCESS:
            _process();
            break;
        default:
            break;
    }
}

void IsometricPositionable::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_local_position_3d", "p_local"), &IsometricPositionable::set_local_position_3d);
    ClassDB::bind_method(D_METHOD("get_local_position_3d"), &IsometricPositionable::get_local_position_3d);
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "local_position_3d"), "set_local_position_3d", "get_local_position_3d");

    ClassDB::bind_method(D_METHOD("set_size", "s"), &IsometricPositionable::set_size);
    ClassDB::bind_method(D_METHOD("get_size"), &IsometricPositionable::get_size);
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "size"), "set_size", "get_size");
    ADD_PROPERTY_DEFAULT("size", Vector3(1, 1, 1));

    ClassDB::bind_method(D_METHOD("set_z_order_size", "size"), &IsometricPositionable::set_z_order_size);
    ClassDB::bind_method(D_METHOD("get_z_order_size"), &IsometricPositionable::get_z_order_size);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "z_order_size"), "set_z_order_size", "get_z_order_size");
    ADD_PROPERTY_DEFAULT("z_order_size", 1);


    //    BIND_ENUM_CONSTANT(NONE);
    //    BIND_ENUM_CONSTANT(LEFT);
    //    BIND_ENUM_CONSTANT(RIGHT);
    //    BIND_ENUM_CONSTANT(FORWARD);
    //    BIND_ENUM_CONSTANT(BACKWARD);
}

void IsometricPositionable::set_debug(bool b) {
    if (outline_drawer) {
        outline_drawer->set_visible(b);
        outline_drawer->update();
    }
}

RID IsometricPositionable::get_space_RID() const{
    return world;
}
