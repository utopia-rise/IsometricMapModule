#ifndef ISOMETRIC_MAPS_ISOMETRIC_MAP_H
#define ISOMETRIC_MAPS_ISOMETRIC_MAP_H

#include "modules/isometric_maps/src/node/isometric_positionable.h"
#include <modules/isometric_maps/src/containers/grid_3d.h>
#include <modules/isometric_maps/src/resource/positionable_set.h>

namespace node {

    class IsometricMap : public IsometricPositionable {
    GDCLASS(IsometricMap, IsometricPositionable)

    private:
        bool draw_tiles;
        containers::Grid3D<int, -1> grid_3d;
        Ref<resource::PositionableSet> positionable_set;

    public:
        Ref<resource::PositionableSet> get_positionable_set() const;

        void set_positionable_set(const Ref<resource::PositionableSet>& set);

        IsometricMap();

        ~IsometricMap() override = default;

    protected:
        static void _bind_methods();
    };
} // namespace positionable
#endif //ISOMETRIC_MAPS_ISOMETRIC_MAP_H
