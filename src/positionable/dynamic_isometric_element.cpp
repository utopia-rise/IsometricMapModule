#include "dynamic_isometric_element.h"

using namespace positionable;

void DynamicIsometricElement::_enter_tree() {
    IsometricElement<physics::DefaultKinematicBody>::_enter_tree();
}

bool DynamicIsometricElement::get_has_default_body() const {
    return IsometricElement<physics::DefaultKinematicBody>::get_has_default_body();
}

void DynamicIsometricElement::set_has_default_body(bool b) {
    IsometricElement<physics::DefaultKinematicBody>::set_has_default_body(b);
}

void DynamicIsometricElement::update_position_from_body(PhysicsBody* physicsBody) {
    IsometricElement<physics::DefaultKinematicBody>::update_position_from_body(physicsBody);
}

void DynamicIsometricElement::_bind_methods() {
    ClassDB::bind_method(D_METHOD("update_position_from_body"), &DynamicIsometricElement::update_position_from_body);

    ClassDB::bind_method(D_METHOD("set_has_default_body"), &DynamicIsometricElement::set_has_default_body);
    ClassDB::bind_method(D_METHOD("get_has_default_body"), &DynamicIsometricElement::get_has_default_body);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "has_default_body"), "set_has_default_body", "get_has_default_body");
    ADD_PROPERTY_DEFAULT("has_default_body", true);
}
