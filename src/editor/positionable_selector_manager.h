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

        void select_positionable_at(node::IsometricMap* map, node::IsometricPositionable* positionable);
        void deselect_positionable_at(node::IsometricMap* map, const Vector3& position);
        void deselect_all(node::IsometricMap* map);

        const Vector<Vector3>& get_selected_for_map(node::IsometricMap* map);
        void set_selected_for_map(node::IsometricMap* map,
                                  const Vector<Vector3>& selected);

        PositionableSelectorManager(const PositionableSelectorManager&) = delete;
        PositionableSelectorManager& operator=(const PositionableSelectorManager&) = delete;

    private:
        Map<node::IsometricMap*, Vector<Vector3>> map_to_selected_positions;

        static void show_outline(node::IsometricPositionable* positionable);

        PositionableSelectorManager();
        ~PositionableSelectorManager() = default;
    };
}


#endif //ISOMETRIC_MAPS_POSITIONABLE_SELECTOR_MANAGER_H

#endif
