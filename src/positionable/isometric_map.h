#ifndef ISOMETRIC_MAPS_ISOMETRIC_MAP_H
#define ISOMETRIC_MAPS_ISOMETRIC_MAP_H


#include <modules/isometric_maps/src/containers/grid_3d.h>
#include <modules/isometric_maps/src/isometric_config.h>
#include "isometric_positionable.h"

namespace positionable {

    class IsometricMap : public IsometricPositionable{
        GDCLASS(IsometricMap, IsometricPositionable)

    private:
        bool draw_tiles;
        containers::Grid3D grid_3d;
        containers::Grid3D edition_grid_3d;

        Array get_flatten_positionables(const Vector3& offset = Vector3());
        void insert_map_as_flatten(IsometricMap* map, const Vector3& offset);
        IsometricMap* initialize_from();

    public:
        Ref<IsometricConfig> isometric_config;

        IsometricMap();
        ~IsometricMap() override = default;

        void _ready();
        void _enter_tree() override;
        void _process(float delta);

        void add_iso_positionable(IsometricPositionable* isometricPositionable);
        void remove_iso_positionable(IsometricPositionable* isometricPositionable);
        IsometricPositionable* get_positionable_at(Vector3 pos, bool onlyLeftUpperCorner = true);
        bool is_overlapping(IsometricPositionable* positionable);
        bool is_overlapping_aabb(AABB aabb);
        bool are_map_elements_overlapping(Vector3 position, IsometricMap* map);
        bool has(IsometricPositionable* isometricPositionable);
        Array getPositionableChildren() const;

        IsometricMap* flatten();

        void on_resize() override;
        void on_grid_updated(int stair) override;
        void set_aabb(AABB ab) override;
        void set_has_moved(bool hm) override;

    protected:
        static void _bind_methods();
    };
}


#endif //ISOMETRIC_MAPS_ISOMETRIC_MAP_H
