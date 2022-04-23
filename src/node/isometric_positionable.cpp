#include "isometric_positionable.h"
#include <modules/isometric_maps/src/isometric_server.h>
#include <modules/isometric_maps/src/utils/isometric_maths.h>
#include <modules/isometric_maps/src/editor/outline_drawer.h>
#include <modules/isometric_maps/src/logging.h>
#include <core/engine.h>

using namespace node;

IsometricPositionable::IsometricPositionable() :
        Node2D(), size({1, 1, 1}), is_dynamic(false), collision_object_node_path(),
        collision_object(nullptr), world(RID()), world_owner(false), self(RID())
#ifdef TOOLS_ENABLED
        , outline_data()
#endif
        {
            set_process(true);
}

void IsometricPositionable::_enter_tree() {
    if (!collision_object_node_path.is_empty()) {
        if (auto* collider{Object::cast_to<CollisionObject>(get_node(collision_object_node_path))}) {
            collision_object = collider;
            _rebind_collision_object_position();
        } else {
            LOG_ERROR(vformat("Positionable %s collision_object_node_path does not point to valid CollisionObject", this->get_instance_id()));
        }
    }

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
    if (collision_object && !Engine::get_singleton()->is_editor_hint()) {
        const Vector3& collision_origin{collision_object->get_global_transform().origin};
        set_global_position_3d({collision_origin.x, collision_origin.z, collision_origin.y});
    }
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
    _rebind_collision_object_position();
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
    _rebind_collision_object_position();
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

RID IsometricPositionable::get_space_RID() const{
    return world;
}

IsometricPositionable::SlopeType IsometricPositionable::get_slope_type() const {
    return slope_type;
}

bool IsometricPositionable::get_is_dynamic() const {
    return is_dynamic;
}

void IsometricPositionable::set_is_dynamic(bool p_is_dynamic) {
    is_dynamic = p_is_dynamic;
}

const NodePath& IsometricPositionable::get_collision_object_node_path() const {
    return collision_object_node_path;
}

void IsometricPositionable::set_collision_object_node_path(const NodePath& p_node_path) {
    collision_object_node_path = p_node_path;
}

void IsometricPositionable::_rebind_collision_object_position() const {
    if (!collision_object) {
        return;
    }
    const Vector3& global_position{get_global_position_3d()};
    collision_object->set_global_transform(
            {
                    {1, 0, 0, 0, 1, 0, 0, 0, 1},
                    {global_position.x, global_position.z, global_position.y}
            }
    );
}

#ifdef TOOLS_ENABLED

editor::OutlineData& IsometricPositionable::get_outline_data() {
    return outline_data;
}

#endif

void IsometricPositionable::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_local_position_3d", "p_local"), &IsometricPositionable::set_local_position_3d);
    ClassDB::bind_method(D_METHOD("get_local_position_3d"), &IsometricPositionable::get_local_position_3d);
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "local_position_3d"), "set_local_position_3d", "get_local_position_3d");

    ClassDB::bind_method(D_METHOD("set_size", "s"), &IsometricPositionable::set_size);
    ClassDB::bind_method(D_METHOD("get_size"), &IsometricPositionable::get_size);
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "size"), "set_size", "get_size");
    ADD_PROPERTY_DEFAULT("size", Vector3(1, 1, 1));

    ClassDB::bind_method(D_METHOD("set_is_dynamic", "p_is_dynamic"), &IsometricPositionable::set_is_dynamic);
    ClassDB::bind_method(D_METHOD("get_is_dynamic"), &IsometricPositionable::get_is_dynamic);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_dynamic"), "set_is_dynamic", "get_is_dynamic");

    ClassDB::bind_method(D_METHOD("set_collision_object_node_path", "p_node_path"), &IsometricPositionable::set_collision_object_node_path);
    ClassDB::bind_method(D_METHOD("get_collision_object_node_path"), &IsometricPositionable::get_collision_object_node_path);
    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "collision_object_node_path"), "set_collision_object_node_path", "get_collision_object_node_path");

    //    BIND_ENUM_CONSTANT(NONE);
    //    BIND_ENUM_CONSTANT(LEFT);
    //    BIND_ENUM_CONSTANT(RIGHT);
    //    BIND_ENUM_CONSTANT(FORWARD);
    //    BIND_ENUM_CONSTANT(BACKWARD);
}