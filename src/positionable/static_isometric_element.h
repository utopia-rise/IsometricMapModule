#ifndef ISOMETRIC_MAPS_STATIC_ISOMETRIC_ELEMENT_H
#define ISOMETRIC_MAPS_STATIC_ISOMETRIC_ELEMENT_H


#include <modules/isometric_maps/src/positionable/physics/default_static_body.h>
#include "isometric_element.h"

namespace positionable {
    class StaticIsometricElement : public IsometricElement<physics::DefaultStaticBody> {
        GDCLASS(StaticIsometricElement, IsometricPositionable)

    private:
        SlopeType slope_type;

    protected:
        SlopeType
        calculate_slope_offset(Vector2* slope_offset, real_t tile_width_float, real_t tile_height_float, real_t width,
                             real_t depth,
                             real_t ratio) const override;

    public:
        StaticIsometricElement();
        ~StaticIsometricElement() override = default;

        void _enter_tree() override;

        SlopeType get_slope_type() const override;
        void set_slope_type(SlopeType type);

        /**
         * Sets if StaticIsometricElement should have default body.
         * @param b
         */
        bool get_has_default_body() const override;
        void set_has_default_body(bool b) override;

        void update_position_from_body(PhysicsBody *physicsBody) override;

    protected:
        static void _bind_methods();
    };
}


#endif //ISOMETRIC_MAPS_STATIC_ISOMETRIC_ELEMENT_H
