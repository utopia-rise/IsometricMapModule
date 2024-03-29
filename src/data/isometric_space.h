#ifndef ISOMETRIC_MAPS_ISOMETRIC_SPACE_H
#define ISOMETRIC_MAPS_ISOMETRIC_SPACE_H

#include "isometric_element.h"
#include "isometric_parameters.h"

namespace data {
    struct IsometricSpace {
        //////Set by commands (isometric server thread)//////
        data::IsometricParameters configuration {data::IsometricParameters::getDefaultConfiguration()};
        LocalVector<data::IsometricElement*> static_elements;
        LocalVector<data::IsometricElement*> dynamic_elements;

        // Whether the space needs reordering or not.
        bool dirty = false;
        // Whether the space data were retrieved or not.
        bool fetched = true;

        IsometricSpace() {
            static_elements.reserve(512);
            dynamic_elements.reserve(32);
        }
    };
}// namespace data

#endif// ISOMETRIC_MAPS_ISOMETRIC_SPACE_H
