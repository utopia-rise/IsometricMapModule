#ifndef ISOMETRIC_MAPS_ISOMETRIC_MAP_H
#define ISOMETRIC_MAPS_ISOMETRIC_MAP_H

#include "containers/grid_3d.h"
#include "node/isometric_positionable.h"
#include "resource/positionable_set.h"

namespace node {

    class IsometricMap : public IsometricPositionable {
        GDCLASS(IsometricMap, IsometricPositionable);

    public:
        static constexpr uint32_t DEFAULT_LAYER_ID = 0;
        static constexpr uint32_t NO_LAYER_ID = 0xffffffff;
        static constexpr const char* DEFAULT_LAYER_NAME = "Default";

#ifdef TOOLS_ENABLED
        static constexpr const char* LAST_EDITED_LAYER_META_NAME = "_LAST_EDITED_LAYER";
        static constexpr const char* LAYER_COLOR_META_NAME_FORMAT = "_LAYER_%s_COLOR";
#endif

    private:
        containers::Grid3D<int, resource::PositionableSet::NONE_POSITIONABLE_ID> grid_3d;
        containers::Grid3D<uint32_t, DEFAULT_LAYER_ID> layers_grid_3d;
        containers::Grid3D<IsometricPositionable*, nullptr> instances_grid_3d;
        Ref<resource::PositionableSet> positionable_set;
        bool child_positionable_initialized;
        Dictionary layers;
        HashMap<uint32_t, StringName> layers_groups;

#ifdef TOOLS_ENABLED
        uint32_t last_layer_id;
#endif

        void _enter_tree() override;
        Array _get_grid_3d() const;
        void _set_grid_3d(const Array& array);
        Array _get_layers_grid_3d() const;
        void _set_layers_grid_3d(const Array& array);
        void add_positionable_as_child(int positionable_id, const Vector3& p_position, uint32_t layer_id);

    public:
        Ref<resource::PositionableSet> get_positionable_set() const;
        void set_positionable_set(const Ref<resource::PositionableSet>& set);

        const Dictionary& get_layers() const;
        void set_layers(const Dictionary& p_layers);

#ifdef TOOLS_ENABLED
        void add_positionable_if_nothing_present(const AABB& aabb, int id, uint32_t layer_id);
        void remove_positionable(const AABB& aabb);
        IsometricPositionable* get_positionable_at(const Vector3& p_position);
        int get_positionable_id_for_position(const Vector3& p_position);
        Vector<IsometricPositionable*> get_positionables_in(const AABB& p_aabb) const;
        void set_layer_id_at(const Vector3& p_position, uint32_t p_layer_id);
        bool is_aabb_in_map(const AABB& aabb) const;
        void set_size(Vector3 p_size) override;
        bool is_overlapping(const AABB& aabb) const;
        uint32_t add_layer(const String& p_layer, uint32_t p_layer_id = NO_LAYER_ID);
        void remove_layer(uint32_t p_layer_id);
        void remove_layer(const String& p_layer_name);
        void set_layer_name(uint32_t p_layer_id, const String& p_layer_name);
        String get_layer_name(uint32_t p_layer_id);
        uint32_t get_layer_id_at(const Vector3& p_position);
        uint32_t get_last_layer_id() const;
        void set_last_layer_id(uint32_t p_last_layer_id);
        void set_layer_visible(uint32_t p_layer_id, bool is_visible);
        void set_layer_color(uint32_t p_layer_id, const Color& p_color);
#endif

        IsometricMap();

        ~IsometricMap() override = default;

    protected:
        static void _bind_methods();
    };
}// namespace node
#endif// ISOMETRIC_MAPS_ISOMETRIC_MAP_H
