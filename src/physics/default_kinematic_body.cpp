#include "default_kinematic_body.h"
#include <core/engine.h>
#include <core/os/input.h>

using namespace physics;

DefaultKinematicBody::DefaultKinematicBody() :
        KinematicBody(), speed(1), gravity(9.8), linear_velocity({0, 0, 0}) {
    initialize_shapes();
}

DefaultKinematicBody::~DefaultKinematicBody() {
    delete_shapes();
}

void DefaultKinematicBody::_enter_tree() {
    if (parent) {
        const Vector3 &parent_position{parent->get_global_position_3d()};
        const Vector3 &parent_size{parent->get_size_3d()};

        set_global_transform({{1,                 0,                 0, 0, 1, 0, 0, 0, 1},
                              {parent_position.x, parent_position.z, parent_position.y}});
        collision_shape->set_global_transform({{1,                                        0, 0, 0, 1, 0, 0, 0, 1},
                                               {parent_position.x + parent_size.x * 0.5f, parent_position.z +
                                                                                          0.5f * parent_size.z,
                                                                                             parent_position.y +
                                                                                             parent_size.y * 0.5f}});
        collision_shape->rotate_x(Math::deg2rad(90.0f));
        update_collision_shapes();
    }
}

void DefaultKinematicBody::_physics_process(float delta) {
    if (parent) {
        if (parent->get_has_moved()) {
            const Vector3 &parent_position{parent->get_global_position_3d()};
            const Vector3 &parent_size{parent->get_size_3d()};

            set_global_transform({{1,                 0,                 0, 0, 1, 0, 0, 0, 1},
                                  {parent_position.x, parent_position.z, parent_position.y}});
            collision_shape->set_global_transform({{1,                                        0, 0, 0, 1, 0, 0, 0, 1},
                                                   {parent_position.x + parent_size.x * 0.5f, parent_position.z +
                                                                                              0.5f * parent_size.z,
                                                                                                 parent_position.y +
                                                                                                 parent_size.y *
                                                                                                 0.5f}});
            collision_shape->rotate_x(Math::deg2rad(90.0f));

            calculate_collision_shape();

            parent->set_has_moved(false);
            return;
        }

        if (Engine::get_singleton()->is_editor_hint()) return;

        Input* input{Input::get_singleton()};

        linear_velocity += {0, -gravity * delta, 0};

        Vector3 horizontal_velocity;
        Vector3 vertical_velocity{0, linear_velocity.y, 0};

        if (input->is_action_pressed("player_goes_forward")) {
            horizontal_velocity += Vector3(-1, 0, -1).normalized() * speed;
        }
        if (input->is_action_pressed("player_goes_backward")) {
            horizontal_velocity += Vector3(1, 0, 1).normalized() * speed;
        }
        if (input->is_action_pressed("player_goes_left")) {
            horizontal_velocity += Vector3(-1, 0, 1).normalized() * speed;
        }
        if (input->is_action_pressed("player_goes_right")) {
            horizontal_velocity += Vector3(1, 0, -1).normalized() * speed;
        }

        linear_velocity = horizontal_velocity + vertical_velocity;

        linear_velocity = move_and_slide(linear_velocity, {0, 1, 0});

        parent->update_position_from_body(this);
    }
}

float DefaultKinematicBody::get_speed() const {
    return speed;
}

void DefaultKinematicBody::set_speed(float s) {
    speed = s;
}

float DefaultKinematicBody::get_gravity() const {
    return gravity;
}

void DefaultKinematicBody::set_gravity(float g) {
    gravity = g;
}

void DefaultKinematicBody::calculate_collision_shape() {
    if (parent) {
        const Vector3 size{parent->get_size_3d()};
        real_t radius = sqrt(static_cast<real_t>(size.x * size.x + size.y * size.y)) * 0.5f;
        real_t height = size.z - radius * 2;
        height = height < 0 ? 0 : height;
        shape->set_radius(radius);
        shape->set_height(height);
        collision_shape->set_shape(shape);
    }
}

void DefaultKinematicBody::update_collision_shapes() {
    calculate_collision_shape();
    if (!collision_shape->get_parent()) {
        add_child(collision_shape);
    }
    collision_shape->set_owner(this);
}

void DefaultKinematicBody::_notification(int notif) {
    switch (notif) {
        case NOTIFICATION_ENTER_TREE:
            _enter_tree();
            break;
        case NOTIFICATION_PHYSICS_PROCESS:
            _physics_process(get_physics_process_delta_time());
            break;
        default:
            break;
    }
}

void DefaultKinematicBody::_bind_methods() {
}
