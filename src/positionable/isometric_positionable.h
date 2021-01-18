#ifndef ISOMETRIC_MAPS_ISOMETRIC_POSITIONABLE_H
#define ISOMETRIC_MAPS_ISOMETRIC_POSITIONABLE_H

#include <scene/2d/node_2d.h>
#include <modules/isometric_maps/src/editor/outline_drawer.h>
#include <modules/isometric_maps/src/variant_casters.h>

class IsometricWorld;

namespace positionable {

    class IsometricPositionable : public Node2D {
        GDCLASS(IsometricPositionable, Node2D)

    public:
        enum class SlopeType {
            NONE = 0,
            LEFT = 1,
            RIGHT = 2,
            FORWARD = 3,
            BACKWARD = 4,
            SLOPE_TYPE_MAX = 5
        };

    private:
        AABB aabb;
        Vector3 local_position;
        int z_order_size;
        bool rendered;

        bool is_temporary;
        int debug_z;

        void _exit_tree();

    protected:
        editor::OutlineDrawer* outline_drawer;

        Vector<Vector2> left_points;
        Vector<Vector2> right_points;
        Vector<Vector2> up_points;
        Vector<Vector2> down_points;

        Vector<Vector2> debug_points;

        IsometricWorld* world;
        bool world_owner;

        void prepare_points();
        virtual SlopeType
        calculate_slope_offset(Vector2* slopeOffset, real_t tileWidthFloat, real_t tileHeightFloat, real_t width,
                               real_t depth,
                               real_t ratio) const;

        void _notification(int notif);
        virtual void _enter_tree();

    public:
        IsometricPositionable();
        ~IsometricPositionable() override = default;

        void set_outline_drawer(Color color, real_t line_size);
        Vector3 get_local_position_3d() const;
        void set_local_position_3d(Vector3 p_local);
        Vector3 get_global_position_3d() const;
        virtual void set_global_position_3d(Vector3 pos);

        AABB get_aabb();
        virtual void set_aabb(AABB ab);
        Vector3 get_size_3d() const;
        void set_size_3d(Vector3 s);
        int get_z_order_size() const;
        void set_z_order_size(int size);
        bool is_rendered() const;
        void set_rendered(bool is_rendered);
        bool get_is_temporary() const;
        void set_is_temporary(bool temp);
        int get_debug_z() const;
        void set_debug_z(int d_z);

        virtual void on_resize();
        virtual void on_grid_updated(int stair);
        virtual void on_select(bool selected);
        virtual bool get_has_moved() const;
        virtual void set_has_moved(bool hm);

    protected:
        static void _bind_methods();
    };
}

MAKE_ENUM_TYPE_INFO(positionable::IsometricPositionable::SlopeType)
template<>
struct VariantCaster<positionable::IsometricPositionable::SlopeType> {

    static _FORCE_INLINE_ positionable::IsometricPositionable::SlopeType cast(const Variant& p_variant) {
        return (positionable::IsometricPositionable::SlopeType) p_variant.operator int();
    }
};

DECLARE_VARIANT_CASTER(positionable::IsometricPositionable)

#endif //ISOMETRIC_MAPS_ISOMETRIC_POSITIONABLE_H
