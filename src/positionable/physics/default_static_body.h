#ifndef ISOMETRIC_MAPS_DEFAULT_STATIC_BODY_H
#define ISOMETRIC_MAPS_DEFAULT_STATIC_BODY_H


#include <scene/3d/physics_body.h>
#include <scene/resources/convex_polygon_shape.h>
#include "default_body.h"

namespace positionable {
    namespace physics {

        class DefaultStaticBody : public StaticBody, public DefaultBody<DefaultStaticBody, ConvexPolygonShape> {
            GDCLASS(DefaultStaticBody, StaticBody)

        private:
            void _enter_tree();
            void _physics_process(float delta);

        protected:
            void calculate_collision_shape() override;
            void update_collision_shapes() override;

            void _notification(int notif);

        public:
            DefaultStaticBody();
            ~DefaultStaticBody() override;

        protected:
            static void _bind_methods();
        };
    }
}


#endif //ISOMETRIC_MAPS_DEFAULT_STATIC_BODY_H
