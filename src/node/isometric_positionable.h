#ifndef ISOMETRIC_MAPS_ISOMETRIC_POSITIONABLE_H
#define ISOMETRIC_MAPS_ISOMETRIC_POSITIONABLE_H

#include <modules/isometric_maps/src/editor/outline_drawer.h>
#include <scene/2d/node_2d.h>
#include <core/method_bind.h>

#define ISO_GROUP "positionables"

namespace node {

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
        Vector3 size;
        Vector3 local_position;
        int z_order_size;

        SlopeType slope_type =  SlopeType::NONE;

        int debug_z;

        RID self;
        bool is_dynamic;

    protected:
        editor::OutlineDrawer* outline_drawer;

        Vector<Vector2> up_points;
        Vector<Vector2> down_points;

        RID world;
        bool world_owner;

        void prepare_points();

        void update_position();

        void _notification(int notif);

        virtual void _enter_tree();
        void _process();
        void _exit_tree();

    public:
        IsometricPositionable();

        ~IsometricPositionable() override = default;

        void set_outline_drawer(Color color, real_t line_size);

        Vector3 get_local_position_3d() const;

        void set_local_position_3d(Vector3 p_local);

        Vector3 get_global_position_3d() const;

        virtual Vector3 get_size() const;

        virtual void set_size(Vector3 p_size);

        int get_z_order_size() const;

        void set_z_order_size(int size);

    protected:
        static void _bind_methods();
    };
} // namespace positionable

MAKE_ENUM_TYPE_INFO(node::IsometricPositionable::SlopeType)

#endif //ISOMETRIC_MAPS_ISOMETRIC_POSITIONABLE_H
