#ifdef TOOLS_ENABLED

#ifndef ISOMETRIC_MAPS_POSITIONABLE_SELECTOR_MANAGER_H
#define ISOMETRIC_MAPS_POSITIONABLE_SELECTOR_MANAGER_H

#include <modules/isometric_maps/src/containers/grid_3d.h>
#include <modules/isometric_maps/src/node/isometric_positionable.h>
#include <modules/isometric_maps/src/node/isometric_map.h>

namespace editor {
    class PositionableSelectorManager {
    public:
        static PositionableSelectorManager& get_instance();

        void select_positionable(node::IsometricMap* map, node::IsometricPositionable* positionable);
        void deselect_positionable_at(node::IsometricMap* map, const Vector3& position);
        void deselect_all(node::IsometricMap* map);

        const containers::Grid3D<node::IsometricPositionable*, nullptr>& get_selected_for_map(node::IsometricMap* map);
        void set_selected_for_map(node::IsometricMap* map,
                                  const containers::Grid3D<node::IsometricPositionable*, nullptr>& selected);

        PositionableSelectorManager(const PositionableSelectorManager&) = delete;
        PositionableSelectorManager& operator=(const PositionableSelectorManager&) = delete;

    private:
        Map<node::IsometricMap*, containers::Grid3D<node::IsometricPositionable*, nullptr>> map_to_selected_positionables;

        static void show_outline(node::IsometricPositionable* positionable);

        PositionableSelectorManager();
        ~PositionableSelectorManager() = default;
    };
}


#endif //ISOMETRIC_MAPS_POSITIONABLE_SELECTOR_MANAGER_H

#endif
