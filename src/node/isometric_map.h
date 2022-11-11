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
        bool child_positionable_initialized;

        void _enter_tree() override;
        Array _get_grid_3d() const;
        void _set_grid_3d(const Array& array);
        void add_positionable_as_child(int positionable_id, const Vector3& position);

    public:
        Ref<resource::PositionableSet> get_positionable_set() const;

        void set_positionable_set(const Ref<resource::PositionableSet>& set);

#ifdef TOOLS_ENABLED
        void add_positionable_if_nothing_present(const AABB& aabb, int id);

        void remove_positionable(const AABB& aabb);

        IsometricPositionable* get_positionable_at(const Vector3& position);

        int get_positionable_id_for_position(const Vector3& position);

        Vector<IsometricPositionable*> get_positionables_in(const AABB& p_aabb) const;

        bool is_aabb_in_map(const AABB& aabb) const;

        void set_size(Vector3 p_size) override;

        bool is_overlapping(const AABB& aabb) const;
#endif

        IsometricMap();

        ~IsometricMap() override = default;

    protected:
        static void _bind_methods();
    };
} // namespace positionable
#endif //ISOMETRIC_MAPS_ISOMETRIC_MAP_H
