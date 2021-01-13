#ifndef ISOMETRIC_MAPS_ISOMETRIC_MAP_H
#define ISOMETRIC_MAPS_ISOMETRIC_MAP_H


#include <modules/isometric_maps/src/containers/grid_3d.h>
#include "isometric_positionable.h"
#include <modules/isometric_maps/src/variant_casters.h>

namespace positionable {

    class IsometricMap : public IsometricPositionable{
        GDCLASS(IsometricMap, IsometricPositionable)

    private:
        bool draw_tiles;
        containers::Grid3D grid_3d;
        containers::Grid3D edition_grid_3d;

        Array get_flatten_positionables(const Vector3& offset = Vector3());
        void insert_map_as_flatten(IsometricMap* map, const Vector3& offset);

        void _ready();
        void _process(float delta);

    protected:
        void _notification(int notif);


    public:
        IsometricMap();
        ~IsometricMap() override = default;

        int tile_width;
        int tile_height;
        int angle;
        float e_z;
        float z_ratio;
        float topological_margin;

        void add_iso_positionable(IsometricPositionable* isometric_positionable);
        void remove_iso_positionable(IsometricPositionable* isometric_positionable);
        IsometricPositionable* get_positionable_at(Vector3 pos, bool only_left_upper_corner = true);
        bool is_overlapping(IsometricPositionable* positionable);
        bool is_overlapping_aabb(AABB aabb);
        bool are_map_elements_overlapping(Vector3 position, IsometricMap* map);
        bool has(IsometricPositionable* isometric_positionable);
        Array get_positionable_children() const;

        void on_resize() override;
        void on_grid_updated(int stair) override;
        void set_aabb(AABB ab) override;
        void set_has_moved(bool hm) override;

    protected:
        static void _bind_methods();
    };
}

DECLARE_VARIANT_CASTER(positionable::IsometricMap)

#endif //ISOMETRIC_MAPS_ISOMETRIC_MAP_H
