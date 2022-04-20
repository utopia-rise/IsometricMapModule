#include "isometric_positionable.h"
#include <modules/isometric_maps/src/isometric_server.h>
#include <modules/isometric_maps/src/utils/isometric_maths.h>
#include <modules/isometric_maps/src/editor/outline_drawer.h>

using namespace node;

IsometricPositionable::IsometricPositionable() :
        Node2D(), size({1, 1, 1}), z_order_size(0), is_dynamic(false), has_moved(false),
        world(RID()), world_owner(false), self(RID())
#ifdef TOOLS_ENABLED
        , outline_data()
#endif
        {
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
                                                                                   is_dynamic, {get_global_position_3d(), size});
                update_position();
                return;
            }
        }
    }

    world = IsometricServer::get_instance()->create_space();
    world_owner = true;


    self = IsometricServer::get_instance()->register_isometric_element(world, this->get_canvas_item(),
                                                                       is_dynamic, {get_global_position_3d(), size});
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
}

void IsometricPositionable::update_position() {
    const data::IsometricParameters* params = IsometricServer::get_instance()->get_space_configuration(world);

    Vector2 position2D = utils::from_3D_to_screen(*params, local_position);
    Transform2D transform = get_transform();
    transform.set_origin(position2D);

    set_transform(transform);

#ifdef TOOLS_ENABLED
    editor::OutlineDrawer::draw_outline(this);
#endif
}

Vector3 IsometricPositionable::get_local_position_3d() const {
    return local_position;
}

void IsometricPositionable::set_local_position_3d(Vector3 p_local) {
    local_position = p_local;
    if(self.is_valid()) {
        IsometricServer::get_instance()->set_isometric_element_position(self, get_global_position_3d());
    }
    if (world.is_valid()) {
        update_position();
    }
    has_moved = true;
}

Vector3 IsometricPositionable::get_global_position_3d() const {
    Vector3 global_position = local_position;

    auto* parent = Object::cast_to<IsometricPositionable>(get_parent());
    if(parent){
        global_position += parent->get_global_position_3d();
    }
    return global_position;
}

void IsometricPositionable::set_global_position_3d(const Vector3& p_position) {
    const Vector3& offset{p_position - get_global_position_3d()};
    set_local_position_3d(local_position + offset);
}

Vector3 IsometricPositionable::get_size() const {
    return size;
}

void IsometricPositionable::set_size(Vector3 s) {
    size = s;
    if(self.is_valid()) {
        IsometricServer::get_instance()->set_isometric_element_size(self, size);
    }
#ifdef TOOLS_ENABLED
    editor::OutlineDrawer::draw_outline(this);
#endif
    has_moved = true;
}

int IsometricPositionable::get_z_order_size() const {
    return z_order_size;
}

void IsometricPositionable::set_z_order_size(int size) {
    z_order_size = size;
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

    ClassDB::bind_method(D_METHOD("set_has_moved", "p_has_moved"), &IsometricPositionable::set_has_moved);
    ClassDB::bind_method(D_METHOD("get_has_moved"), &IsometricPositionable::get_has_moved);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "has_moved", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NOEDITOR), "set_has_moved", "get_has_moved");

    //    BIND_ENUM_CONSTANT(NONE);
    //    BIND_ENUM_CONSTANT(LEFT);
    //    BIND_ENUM_CONSTANT(RIGHT);
    //    BIND_ENUM_CONSTANT(FORWARD);
    //    BIND_ENUM_CONSTANT(BACKWARD);
}

RID IsometricPositionable::get_space_RID() const{
    return world;
}

IsometricPositionable::SlopeType IsometricPositionable::get_slope_type() const {
    return slope_type;
}

bool IsometricPositionable::get_has_moved() const {
    return has_moved;
}

void IsometricPositionable::set_has_moved(bool p_has_moved) {
    has_moved = p_has_moved;
}

#ifdef TOOLS_ENABLED

editor::OutlineData& IsometricPositionable::get_outline_data() {
    return outline_data;
}

#endif