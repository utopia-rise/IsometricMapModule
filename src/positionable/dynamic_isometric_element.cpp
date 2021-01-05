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

}
