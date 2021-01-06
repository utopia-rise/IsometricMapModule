#include "static_isometric_element.h"

using namespace positionable;

StaticIsometricElement::StaticIsometricElement()
        : IsometricElement<physics::DefaultStaticBody>(), slope_type(SlopeType::NONE) {

}

void StaticIsometricElement::_enter_tree() {
    IsometricElement<physics::DefaultStaticBody>::_enter_tree();
}

IsometricPositionable::SlopeType StaticIsometricElement::get_slope_type() const {
    return slope_type;
}

void StaticIsometricElement::set_slope_type(SlopeType type) {
    slope_type = type;
    if (outline_drawer) {
        set_outline_drawer(outline_drawer->get_color(), outline_drawer->get_line_size());
    }
    has_moved = true;
    update();
}

bool StaticIsometricElement::get_has_default_body() const {
    return IsometricElement<physics::DefaultStaticBody>::get_has_default_body();
}

void StaticIsometricElement::set_has_default_body(bool b) {
    IsometricElement<physics::DefaultStaticBody>::set_has_default_body(b);
}

void StaticIsometricElement::update_position_from_body(PhysicsBody* physicsBody) {

}

IsometricPositionable::SlopeType
StaticIsometricElement::calculate_slope_offset(Vector2* slope_offset, real_t tile_width_float, real_t tile_height_float,
                                               real_t width, real_t depth, real_t ratio) const {
    switch (slope_type) {
        case SlopeType::NONE:
            break;
        case SlopeType::LEFT:
            *slope_offset = -Vector2(tile_width_float * 0.5f * width, tile_height_float * 0.5f * width) * ratio;
            break;
        case SlopeType::RIGHT:
            *slope_offset = Vector2(tile_width_float * 0.5f * width, tile_height_float * 0.5f * width) * ratio;
            break;
        case SlopeType::FORWARD:
            *slope_offset = -Vector2(-tile_width_float * 0.5f * depth, tile_height_float * 0.5f * depth) * ratio;
            break;
        case SlopeType::BACKWARD:
            *slope_offset = Vector2(-tile_width_float * 0.5f * depth, tile_height_float * 0.5f * depth) * ratio;
            break;
    }
    return slope_type;
}

void StaticIsometricElement::_bind_methods() {
    ClassDB::bind_method(D_METHOD("has_moved"), &StaticIsometricElement::get_has_moved);
    ClassDB::bind_method(D_METHOD("set_has_moved"), &StaticIsometricElement::set_has_moved);

    ClassDB::bind_method(D_METHOD("set_slope_type"), &StaticIsometricElement::set_slope_type);
    ClassDB::bind_method(D_METHOD("get_slope_type"), &StaticIsometricElement::get_slope_type);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "slope_type", PROPERTY_HINT_ENUM, "NONE,LEFT,RIGHT,FORWARD,BACKWARD"),
            "set_slope_type", "get_slope_type");
    ADD_PROPERTY_DEFAULT("slope_type", static_cast<int>(SlopeType::NONE));

    ClassDB::bind_method(D_METHOD("set_has_default_body"), &StaticIsometricElement::set_has_default_body);
    ClassDB::bind_method(D_METHOD("get_has_default_body"), &StaticIsometricElement::get_has_default_body);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "has_default_body"), "set_has_default_body", "get_has_default_body");
    ADD_PROPERTY_DEFAULT("has_default_body", true);
}
