#ifndef ISOMETRIC_MAPS_ISOMETRIC_KINEMATIC_BODY_H
#define ISOMETRIC_MAPS_ISOMETRIC_KINEMATIC_BODY_H


#include <scene/3d/physics_body.h>
#include "isometric_body_base.h"

namespace node {
    namespace physics {
        class IsometricKinematicBody : public KinematicBody, public IsometricBodyBase<IsometricKinematicBody, true, true>{
            friend class IsometricBodyBase<IsometricKinematicBody, true, true>;
            GDCLASS(IsometricKinematicBody, KinematicBody)
        public:
            void _notification(int notif);
            static void _bind_methods();
        };
    }
}


#endif //ISOMETRIC_MAPS_ISOMETRIC_KINEMATIC_BODY_H
