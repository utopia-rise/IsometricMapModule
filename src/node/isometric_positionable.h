#ifndef ISOMETRIC_MAPS_ISOMETRIC_POSITIONABLE_H
#define ISOMETRIC_MAPS_ISOMETRIC_POSITIONABLE_H

#include "editor/outline_data.h"
#include "scene/2d/node_2d.h"
#include "scene/3d/collision_object_3d.h"
#include "scene/3d/mesh_instance_3d.h"
#include "scene/resources/primitive_meshes.h"

namespace node {
    class IsometricPositionable : public Node2D {
        GDCLASS(IsometricPositionable, Node2D);

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
        int depth;

        SlopeType slope_type = SlopeType::NONE;

        RID self;
        bool is_dynamic;
        NodePath collision_object_node_path;
        CollisionObject3D* collision_object;

        void set_global_position_3d(const Vector3& p_position);

        void _rebind_collision_object_position() const;

#ifdef TOOLS_ENABLED
        bool debug_view = false;
        editor::OutlineData outline_data;

        MeshInstance3D* debug_mesh_instance_3d;
        Ref<BoxMesh> debug_mesh;
        Ref<StandardMaterial3D> debug_mesh_material;

        void _rebind_debug_mesh_instance() const;
#endif

    protected:
        RID world;
        bool world_owner;
        bool is_container;

        void update_position();

        void _notification(int notif);

        virtual void _enter_tree();
        void _ready();
        void _physics_process();
        void _exit_tree();
        void _on_pre_delete();

    public:
        IsometricPositionable();
        ~IsometricPositionable() override;

        Vector3 get_local_position_3d() const;
        void set_local_position_3d(Vector3 p_local);
        Vector3 get_global_position_3d() const;
        virtual Vector3 get_size() const;
        virtual void set_size(Vector3 p_size);
        int get_depth() const;
        void set_depth(int p_depth);
        RID get_space_RID() const;
        SlopeType get_slope_type() const;
        bool get_is_dynamic() const;
        void set_is_dynamic(bool p_is_dynamic);
        const NodePath& get_collision_object_node_path() const;
        void set_collision_object_node_path(const NodePath& p_node_path);
        RID get_rid() const;

#ifdef TOOLS_ENABLED

        void set_debug_view(bool p_debug);

        editor::OutlineData& get_outline_data();
        void update_debug_mesh_color(const Color& p_color) const;
#endif
#ifdef DEBUG_ENABLED
        virtual void set_debug_modulate(const Color &p_modulate) const;

        static Color CONFLICT_MODULATE_COLOR;
#endif

    protected:
        static void _bind_methods();
    };
}// namespace node

MAKE_ENUM_TYPE_INFO(node::IsometricPositionable::SlopeType)

#endif// ISOMETRIC_MAPS_ISOMETRIC_POSITIONABLE_H
