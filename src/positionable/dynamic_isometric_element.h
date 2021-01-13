#ifndef ISOMETRIC_MAPS_DYNAMIC_ISOMETRIC_ELEMENT_H
#define ISOMETRIC_MAPS_DYNAMIC_ISOMETRIC_ELEMENT_H


#include <modules/isometric_maps/src/positionable/physics/default_kinematic_body.h>
#include "isometric_element.h"
#include <modules/isometric_maps/src/variant_casters.h>

namespace positionable {

    class DynamicIsometricElement : public IsometricElement<physics::DefaultKinematicBody>{
        GDCLASS(DynamicIsometricElement, IsometricPositionable)

    private:
        void _enter_tree() override;

    public:
        DynamicIsometricElement() = default;
        ~DynamicIsometricElement() override = default;

        bool get_has_default_body() const override;
        void set_has_default_body(bool b) override;

        void update_position_from_body(PhysicsBody *physics_body) override;

    protected:
        static void _bind_methods();
    };
}

#endif //ISOMETRIC_MAPS_DYNAMIC_ISOMETRIC_ELEMENT_H
