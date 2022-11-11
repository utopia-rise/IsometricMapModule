//
// Created by CedNaru on 14/02/2021.
//

#ifndef ISOMETRIC_MAPS_ISOMETRIC_PARAMETERS_H
#define ISOMETRIC_MAPS_ISOMETRIC_PARAMETERS_H

#include <core/object.h>
#include <core/resource.h>

#define DEFAULT_ANGLE 30
#define DEFAULT_WIDTH 256
#define DEFAULT_MARGIN 0.1f

namespace data {
    struct IsometricParameters {
        int diamond_width;
        int diamond_height;
        float z_length;
        float z_ratio;
        float topological_margin;

        IsometricParameters() = delete;

        IsometricParameters(int p_tile_width, int p_angle, float p_topological_margin);

        static IsometricParameters getDefaultConfiguration();
    };
}// namespace data
#endif// ISOMETRIC_MAPS_ISOMETRIC_PARAMETERS_H
