#include "isometric_space.h"
#include "isometric_parameters.h"

using namespace data;

IsometricSpace::IsometricSpace() :
        configuration{data::IsometricParameters::getDefaultConfiguration()},
        static_elements{},
        dynamic_elements{} {

}