#ifndef ISOMETRIC_MAPS_ISOMETRIC_STATIC_BODY_H
#define ISOMETRIC_MAPS_ISOMETRIC_STATIC_BODY_H


#include <scene/3d/physics_body.h>
#include "isometric_body_base.h"

namespace node {
    namespace physics {
        class IsometricStaticBody : public StaticBody, public IsometricBodyBase<IsometricStaticBody, false, false>{
            friend class IsometricBodyBase<IsometricStaticBody, false, false>;
            GDCLASS(IsometricStaticBody, StaticBody)

        public:
            void _notification(int notif);
            static void _bind_methods();
        };
    }
}


#endif //ISOMETRIC_MAPS_ISOMETRIC_STATIC_BODY_H
