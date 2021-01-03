#ifndef ISOMETRIC_MAPS_ISOMETRIC_CONFIG_H
#define ISOMETRIC_MAPS_ISOMETRIC_CONFIG_H


#include <core/resource.h>

class IsometricConfig : public Resource {
    GDCLASS(IsometricConfig, Resource)

private:
    int tile_width;
    int tile_height;
    int angle;
    float e_z;
    float z_ratio;
    float topological_margin;

public:
    int get_tile_width() const;
    void set_tile_width(int tW);
    int get_tile_height() const;
    int get_angle() const;
    void set_angle(int agl);

    float get_e_z() const;
    double get_z_ratio() const;

    float get_topological_margin() const;
    void set_topological_margin(float margin);

protected:
    static void _bind_methods();
};


#endif //ISOMETRIC_MAPS_ISOMETRIC_CONFIG_H
