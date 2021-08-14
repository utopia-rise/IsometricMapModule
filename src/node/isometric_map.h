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
        containers::Grid3D<int, resource::PositionableSet::NONE_POSITIONABLE_ID> grid_3d;
        containers::Grid3D<IsometricPositionable*, nullptr> instances_grid_3d;
        Ref<resource::PositionableSet> positionable_set;

        void _on_enter_tree();
        Array _get_grid_3d() const;
        void _set_grid_3d(const Array& array);
        void add_positionable_as_child(int positionable_id, const Vector3& position);

    protected:
        void _notification(int notif);

    public:
        Ref<resource::PositionableSet> get_positionable_set() const;

        void set_positionable_set(const Ref<resource::PositionableSet>& set);

#ifdef TOOLS_ENABLED
        void add_positionable_if_nothing_present(const Vector3& position, int id);

        void remove_positionable(const Vector3& position);

        Object* get_positionable_at(const Vector3& position);

        bool is_position_in_map(const Vector3& p_position) const;

        void set_size(Vector3 p_size) override;
#endif

        IsometricMap();

        ~IsometricMap() override = default;

    protected:
        static void _bind_methods();
    };
} // namespace positionable
#endif //ISOMETRIC_MAPS_ISOMETRIC_MAP_H
