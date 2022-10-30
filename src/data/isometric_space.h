#ifndef ISOMETRIC_MAPS_ISOMETRIC_SPACE_H
#define ISOMETRIC_MAPS_ISOMETRIC_SPACE_H

#include <core/rid.h>
#include "isometric_parameters.h"

namespace data {
    struct IsometricSpace : public RID_Data {
        data::IsometricParameters configuration;
        Vector<data::IsometricElement*> static_elements;
        Vector<data::IsometricElement*> dynamic_elements;

        IsometricSpace();
    };
} // namespace data

#endif //ISOMETRIC_MAPS_ISOMETRIC_SPACE_H
