#ifndef ISOMETRIC_MAPS_ISOMETRIC_API_H
#define ISOMETRIC_MAPS_ISOMETRIC_API_H

#include <core/object.h>
#include <core/resource.h>
#include "../data/isometric_positionable_data.h"

#define DEFAULT_ANGLE 30
#define DEFAULT_WIDTH 265
#define DEFAULT_MARGIN 0.1f

namespace resource {

    struct IsometricConfiguration : public Resource {
    GDCLASS(IsometricConfiguration, Resource)

    private:
        struct Hexagone {
            real_t minX;
            real_t maxX;
            real_t minY;
            real_t maxY;
            real_t hMin;
            real_t hMax;
        };
        int tile_width;
        int tile_height;
        int angle;
        float e_z;
        float z_ratio;
        float topological_margin;

        ~IsometricConfiguration() override = default;

    public:
        IsometricConfiguration();

        IsometricConfiguration(const IsometricConfiguration &) = delete;

        static IsometricConfiguration* getDefaultConfiguration();

    private:
        int calculate_tile_height() const;

        float calculate_ez() const;

        Hexagone get_hexagone_2D_coordinates(const data::IsometricPositionableData* data) const;

    public:
        int get_angle() const;

        void set_angle(int agl);

        int get_tile_width() const;

        void set_tile_width(int t_w);

        float get_topological_margin() const;

        void set_topological_margin(float margin);

        int get_tile_height() const;

        float get_e_z() const;

        float get_z_ratio() const;

        Vector2 get_screen_coord_from_3d(const Vector3 &pos) const;

        Vector3 get_3d_coord_from_screen(const Vector2 &pos, real_t orth_z) const;

        bool do_iso_elements_overlap(const data::IsometricPositionableData* data1,
                                     const data::IsometricPositionableData* data2);

        bool is_box_in_front(const AABB &box, const AABB &other);

    protected:
        static void _bind_methods();
    };

} // namespace resource

#endif //ISOMETRIC_MAPS_ISOMETRIC_API_H
