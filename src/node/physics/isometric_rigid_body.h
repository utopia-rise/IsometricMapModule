#ifndef ISOMETRIC_MAPS_ISOMETRIC_RIGID_BODY_H
#define ISOMETRIC_MAPS_ISOMETRIC_RIGID_BODY_H


#include <scene/3d/physics_body.h>
#include "isometric_body_base.h"

namespace node {
    namespace physics {
        class IsometricRigidBody : public RigidBody, IsometricBodyBase<IsometricRigidBody, false, true>{
            friend class IsometricBodyBase<IsometricRigidBody, false, true>;
            GDCLASS(IsometricRigidBody, RigidBody)

        public:
            void _notification(int notif);
            static void bind_methods();
        };
    }
}


#endif //ISOMETRIC_MAPS_ISOMETRIC_RIGID_BODY_H
