#ifndef ISOMETRIC_MAPS_CHUNK_H
#define ISOMETRIC_MAPS_CHUNK_H

#include <core/templates/local_vector.h>
#include "isometric_element.h"

namespace data {
    struct Chunk {
        LocalVector<IsometricElement*> static_elements;
        LocalVector<IsometricElement*> dynamic_elements;

        LocalVector<Chunk*> all_neighbors;
        LocalVector<Chunk*> dynamic_sorting_neighbors;

        Chunk() {
            all_neighbors.reserve(9);
            dynamic_sorting_neighbors.reserve(5);
            static_elements.reserve(256);
            dynamic_elements.reserve(16);
        }
    };
}

#endif// ISOMETRIC_MAPS_CHUNK_H
