#ifndef ISOMETRIC_MAPS_DEFAULT_BODY_H
#define ISOMETRIC_MAPS_DEFAULT_BODY_H

#include <modules/isometric_maps/src/positionable/isometric_element.h>
#include <scene/3d/collision_shape.h>

namespace physics {

    template<class T, class U>
    class DefaultBody {

    protected:
        IsometricElement <T>* parent;
        U* shape;
        CollisionShape* collision_shape;

        void initialize_shapes();

        void delete_shapes();

        virtual void calculate_collision_shape();

        virtual void update_collision_shapes();

    public:
        DefaultBody();

        ~DefaultBody() = default;

        /**
             * Sets the parent node of this DefaultBody. Should be an IsometricElement.
             * @param isometric_element
             */
        void set_parent(IsometricElement <T>* isometric_element);
    };

    template<class T, class U>
    DefaultBody<T, U>::DefaultBody() :
            parent(nullptr), shape(nullptr), collision_shape(nullptr) {
    }

    template<class T, class U>
    void DefaultBody<T, U>::set_parent(IsometricElement <T>* isometric_element) {
        parent = isometric_element;
    }

    template<class T, class U>
    void DefaultBody<T, U>::calculate_collision_shape() {
    }

    template<class T, class U>
    void DefaultBody<T, U>::update_collision_shapes() {
    }

    template<class T, class U>
    void DefaultBody<T, U>::initialize_shapes() {
        collision_shape = memnew(CollisionShape());
        shape = memnew(U());
    }

    template<class T, class U>
    void DefaultBody<T, U>::delete_shapes() {
        memdelete(collision_shape);
        collision_shape = nullptr;
        memdelete(shape);
        shape = nullptr;
    }
} // namespace physics

#endif //ISOMETRIC_MAPS_DEFAULT_BODY_H
