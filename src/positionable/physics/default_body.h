#ifndef ISOMETRIC_MAPS_DEFAULT_BODY_H
#define ISOMETRIC_MAPS_DEFAULT_BODY_H

#include <modules/isometric_maps/src/positionable/isometric_element.h>
#include <scene/3d/collision_shape.h>

namespace positionable {
    namespace physics {

        template<class T, class U>
        class DefaultBody {

        protected:
            IsometricElement<T>* parent;
            U* shape;
            CollisionShape* collisionShape;

            void initialize_shapes();

            virtual void calculate_collision_shape();

            virtual void update_collision_shapes();

        public:
            DefaultBody();

            ~DefaultBody() = default;

            /**
             * Sets the parent node of this DefaultBody. Should be an IsometricElement.
             * @param isometricElement
             */
            void set_parent(IsometricElement<T>* isometricElement);
        };

        template<class T, class U>
        DefaultBody<T, U>::DefaultBody(): parent(nullptr), shape(nullptr), collisionShape(nullptr) {

        }

        template<class T, class U>
        void DefaultBody<T, U>::set_parent(IsometricElement<T>* isometricElement) {
            parent = isometricElement;
        }

        template<class T, class U>
        void DefaultBody<T, U>::calculate_collision_shape() {

        }

        template<class T, class U>
        void DefaultBody<T, U>::update_collision_shapes() {

        }

        template<class T, class U>
        void DefaultBody<T, U>::initialize_shapes() {
            collisionShape = CollisionShape::_new();
            shape = U::_new();
        }
    }
}

#endif //ISOMETRIC_MAPS_DEFAULT_BODY_H
