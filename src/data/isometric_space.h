#ifndef ISOMETRIC_MAPS_ISOMETRIC_SPACE_H
#define ISOMETRIC_MAPS_ISOMETRIC_SPACE_H

#include "chunk.h"
#include "isometric_element.h"
#include "isometric_parameters.h"

namespace data {
    struct IsometricSpace {
        //////Set by commands (isometric server thread)//////
        data::IsometricParameters configuration {data::IsometricParameters::getDefaultConfiguration()};
        HashMap<Vector2i, Chunk> chunks;
        LocalVector<IsometricElement*> statics_elements;
        LocalVector<IsometricElement*> dynamics_elements;

        Vector2i cell_size = Vector2i(10, 10);

        // Whether the space needs reordering or not.
        bool dirty = false;
        // Whether the space data were retrieved or not.
        bool fetched = true;
    };
}// namespace data

#endif// ISOMETRIC_MAPS_ISOMETRIC_SPACE_H
