#ifndef ISOMETRIC_MAPS_DEFAULT_KINEMATIC_BODY_H
#define ISOMETRIC_MAPS_DEFAULT_KINEMATIC_BODY_H


#include <scene/3d/physics_body.h>
#include <scene/resources/capsule_shape.h>
#include "default_body.h"

namespace positionable {
    namespace physics {

        class DefaultKinematicBody : public KinematicBody, public DefaultBody<DefaultKinematicBody, CapsuleShape>{
            GDCLASS(DefaultKinematicBody, KinematicBody)

        private:
            float speed;
            float gravity;

            Vector3 linear_velocity;

            void _enter_tree();
            void _physics_process(float delta);

        protected:
            void calculate_collision_shape() override;
            void update_collision_shapes() override;

            void _notification(int notif);

        public:
            DefaultKinematicBody();
            ~DefaultKinematicBody() override;

            float get_speed() const;
            void set_speed(float s);

            float get_gravity() const;
            void set_gravity(float g);

        protected:
            static void _bind_methods();
        };
    }
}


#endif //ISOMETRIC_MAPS_DEFAULT_KINEMATIC_BODY_H
