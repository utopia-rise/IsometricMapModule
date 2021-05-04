#ifndef ISOMETRIC_MAPS_ISOMETRIC_MAP_H
#define ISOMETRIC_MAPS_ISOMETRIC_MAP_H

#include "modules/isometric_maps/src/node/isometric_positionable.h"
#include <modules/isometric_maps/src/containers/grid_3d.h>

namespace node {

    class IsometricMap : public IsometricPositionable {
    GDCLASS(IsometricMap, IsometricPositionable)

    private:
        bool draw_tiles;
        containers::Grid3D grid_3d;
        containers::Grid3D edition_grid_3d;

        Array get_flatten_positionables(const Vector3 &offset = Vector3());

        void insert_map_as_flatten(Node* map, const Vector3 &offset);

        void _ready();

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

        void add_iso_positionable(Node* isometric_node);

        void remove_iso_positionable(Node* isometric_node);

        Node* get_positionable_at(Vector3 pos, bool only_left_upper_corner = true);

        bool is_overlapping(Node* node);

        bool is_overlapping_aabb(AABB aabb);

        bool are_map_elements_overlapping(Vector3 position, Node* map);

        bool has(Node* isometric_node);

        Array get_positionable_children() const;

    protected:
        static void _bind_methods();
    };
} // namespace positionable
#endif //ISOMETRIC_MAPS_ISOMETRIC_MAP_H
