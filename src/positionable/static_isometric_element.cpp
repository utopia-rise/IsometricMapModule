#include "static_isometric_element.h"

using namespace positionable;

StaticIsometricElement::StaticIsometricElement()
        : IsometricElement<physics::DefaultStaticBody>(), slope_type(SlopeType::NONE) {

}
