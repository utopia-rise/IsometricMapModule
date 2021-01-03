#ifndef ISOMETRIC_MAPS_DYNAMIC_ISOMETRIC_ELEMENT_H
#define ISOMETRIC_MAPS_DYNAMIC_ISOMETRIC_ELEMENT_H


#include <modules/isometric_maps/src/positionable/physics/default_kinematic_body.h>
#include "isometric_element.h"

namespace positionable {

    class DynamicIsometricElement : public IsometricElement<physics::DefaultKinematicBody>{
        GDCLASS(DynamicIsometricElement, IsometricPositionable)

    public:
        DynamicIsometricElement() = default;
        ~DynamicIsometricElement() override = default;

        void _enter_tree() override;

        bool get_has_default_body() const override;
        void set_has_default_body(bool b) override;

        void update_position_from_body(PhysicsBody *physicsBody) override;

    protected:
        static void _bind_methods();
    };
}


#endif //ISOMETRIC_MAPS_DYNAMIC_ISOMETRIC_ELEMENT_H
