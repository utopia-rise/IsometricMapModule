#include "isometric_parameters.h"

#include "core/math/math_funcs.h"

using namespace data;

int calculate_tile_height(int tile_width, int angle) {
    return static_cast<int>(round(tile_width * sin(Math::deg_to_rad(static_cast<double>(angle)))));
}

float calculate_ez(int tile_width, int angle) {
    return static_cast<float>((tile_width / Math::sqrt(2.0)) * cos(Math::deg_to_rad(static_cast<double>(angle))));
}

IsometricParameters::IsometricParameters(int p_tile_width, int p_angle, float p_topological_margin) :
  diamond_width {p_tile_width},
  topological_margin {p_topological_margin},
  diamond_height {calculate_tile_height(p_tile_width, p_angle)},
  z_length {calculate_ez(p_tile_width, p_angle)},
  z_ratio {z_length / static_cast<float>(diamond_height)} {}

IsometricParameters IsometricParameters::getDefaultConfiguration() {
    // Todo: Change it so we can set the default values in the editor
    static IsometricParameters singleton(DEFAULT_WIDTH, DEFAULT_ANGLE, DEFAULT_MARGIN);
    return singleton;
}
