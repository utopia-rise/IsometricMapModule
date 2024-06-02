#include "isometric_map.h"

using namespace node;

IsometricMap::IsometricMap() : child_positionable_initialized(false)
#ifdef TOOLS_ENABLED
, last_layer_id(0)
#endif
{
    is_container = true;
}

Ref<resource::PositionableSet> IsometricMap::get_positionable_set() const {
    return positionable_set;
}

void IsometricMap::set_positionable_set(const Ref<resource::PositionableSet>& set) {
    positionable_set = set;
    if (positionable_set.is_valid() && !positionable_set->is_set_loaded()) { positionable_set->preload_scenes(); }
    emit_signal("positional_set_changed", positionable_set);
}

const Dictionary& IsometricMap::get_layers() const {
    return layers;
}

void IsometricMap::set_layers(const Dictionary& p_layers) {
    layers = p_layers;
    Array layer_ids {layers.keys()};
    Array layer_names {layers.values()};
    for (int i = 0; i < layer_ids.size(); ++i) {
        layers_groups[layer_ids[i]] = vformat("iso-map-layer-%s", layer_names[i]);
    }
}

#ifdef TOOLS_ENABLED

void IsometricMap::add_positionable(const Vector3& position, int id, uint32_t layer_id) {
    grid_3d.set_data(position, id);
    set_layer_id_at(position, layer_id);
    add_positionable_as_child(id, position, layer_id);
}

void IsometricMap::add_positionable_if_nothing_present(const AABB& aabb, int id, uint32_t layer_id) {
    if (instances_grid_3d.is_overlapping(aabb)) { return; }

    add_positionable(aabb.position, id, layer_id);
}

void IsometricMap::remove_positionable(const AABB& aabb) {
    IsometricPositionable* element_to_remove {instances_grid_3d.get_data(aabb.position)};
    grid_3d.set_data(aabb.position, containers::Grid3D<int, resource::PositionableSet::NONE_POSITIONABLE_ID>::get_default_value());
    instances_grid_3d.insert_box(aabb, nullptr, true);

#ifdef DEBUG_ENABLED
    Vector<node::IsometricPositionable*> instances_to_reset;
    for (node::IsometricPositionable* conflict_instance : conflict_instances_map[aabb.position]) {
        const Vector3& conflict_instance_position {conflict_instance->get_local_position_3d()};

        instances_to_reset.push_back(conflict_instance);

        conflict_instances_map[conflict_instance_position].erase(element_to_remove);
        if (conflict_instances_map[conflict_instance_position].is_empty()) {
            conflict_instance->set_debug_modulate(conflict_instance->get_modulate());
            instances_grid_3d.insert_box(
              {conflict_instance_position, conflict_instance->get_size()},
              conflict_instance
            );
        }
    }

    conflict_instances_map.erase(aabb.position);

#endif

    set_layer_id_at(aabb.position, DEFAULT_LAYER_ID);
    remove_child(element_to_remove);
    element_to_remove->queue_free();
}

IsometricPositionable* IsometricMap::get_positionable_at(const Vector3& p_position) {
    return instances_grid_3d.get_data(p_position);
}

int IsometricMap::get_positionable_id_for_position(const Vector3& p_position) {
    return grid_3d.get_data(p_position);
}

Vector<IsometricPositionable*> IsometricMap::get_positionables_in(const AABB& p_aabb) const {
    return instances_grid_3d.get_box(p_aabb);
}

bool IsometricMap::has_positionable_in(const AABB& p_aabb, node::IsometricPositionable* p_excluded) const {
    const Vector<IsometricPositionable*>& positionables_in = get_positionables_in(p_aabb);
    for (const node::IsometricPositionable* positionable : positionables_in) {
        if (!positionable) { continue; }
        if (positionable == p_excluded) { continue; }
        return true;
    }

    return false;
}

void IsometricMap::set_layer_id_at(const Vector3& p_position, uint32_t p_layer_id) {
    layers_grid_3d.set_data(p_position, p_layer_id);
}

bool IsometricMap::is_aabb_in_map(const AABB& aabb) const {
    const Vector3& aabb_position {aabb.position};
    const Vector3& aabb_size {aabb.size};
    int pos_x {static_cast<int>(aabb_position.x)};
    int pos_y {static_cast<int>(aabb_position.y)};
    int pos_z {static_cast<int>(aabb_position.z)};
    int max_x {pos_x + static_cast<int>(aabb_size.x) - 1};
    int max_y {pos_y + static_cast<int>(aabb_size.y) - 1};
    int max_z {pos_z + static_cast<int>(aabb_size.z) - 1};

    if (pos_x < 0 || pos_y < 0 || pos_z < 0 || max_x >= grid_3d.get_width() || max_y >= grid_3d.get_depth()
        || max_z >= grid_3d.get_height()) {
        return false;
    }
    return true;
}

void IsometricMap::set_size(Vector3 p_size) {
    IsometricPositionable::set_size(p_size);
    grid_3d.update_array_size(p_size);
    instances_grid_3d.update_array_size(p_size);
    layers_grid_3d.update_array_size(p_size);
}

bool IsometricMap::is_overlapping(const AABB& aabb) const {
    return instances_grid_3d.is_overlapping(aabb);
}

uint32_t IsometricMap::add_layer(const String& p_layer, uint32_t p_layer_id) {
    if (layers.values().has(p_layer)) {
        return NO_LAYER_ID;
    }
    uint32_t layer_id {p_layer_id == NO_LAYER_ID ? ++last_layer_id : p_layer_id};
    layers[layer_id] = p_layer;
    layers_groups[layer_id] = vformat("iso-map-layer-%s", p_layer);
    return layer_id;
}

void IsometricMap::remove_layer(uint32_t p_layer_id) {
    if (p_layer_id == DEFAULT_LAYER_ID) {
        return; // Never remove default layer.
    }

    layers.erase(p_layer_id);
    layers_groups.erase(p_layer_id);
}

void IsometricMap::remove_layer(const String& p_layer_name) {
    uint32_t layer_id = layers.find_key(p_layer_name);
    remove_layer(layer_id);
}

void IsometricMap::set_layer_name(uint32_t p_layer_id, const String& p_layer_name) {
    layers[p_layer_id] = p_layer_name;
}

String IsometricMap::get_layer_name(uint32_t p_layer_id) {
    return layers[p_layer_id];
}

uint32_t IsometricMap::get_layer_id_at(const Vector3& p_position) {
    return layers_grid_3d.get_data(p_position);
}

uint32_t IsometricMap::get_last_layer_id() const {
    return last_layer_id;
}

void IsometricMap::set_last_layer_id(uint32_t p_last_layer_id) {
    last_layer_id = p_last_layer_id;
}

void IsometricMap::set_layer_visible(uint32_t p_layer_id, bool is_visible) {
    List<Node*> nodes_in_layer;
    get_tree()->get_nodes_in_group(layers_groups[p_layer_id], &nodes_in_layer);

    for (Node* node : nodes_in_layer) {
        if (IsometricPositionable* positionable {dynamic_cast<IsometricPositionable*>(node)}) {
            positionable->set_visible(is_visible);
        }
    }
}

void IsometricMap::set_layer_color(const uint32_t p_layer_id, const Color& p_color) {
    set_meta(vformat(node::IsometricMap::LAYER_COLOR_META_NAME_FORMAT, p_layer_id), p_color);

    if (!Engine::get_singleton()->is_editor_hint()) return;

    const Vector<IsometricPositionable*>& instances {instances_grid_3d.get_internal_array()};
    const Vector<uint32_t>& layer_assignations {layers_grid_3d.get_internal_array()};
    for (int i = 0; i < instances.size(); ++i) {
        if (IsometricPositionable* instance {instances[i]}) {
            if (layer_assignations[i] != p_layer_id) continue;

            instance->update_debug_mesh_color(p_color);
        }
    }
}

Vector<Vector3> IsometricMap::get_layer_positions(uint32_t p_layer_id) const {
    Vector<Vector3> positions;
    
    const Vector<uint32_t>& layers_vector {layers_grid_3d.get_internal_array()};
    for(int i = 0; i < layers_vector.size(); ++i) {
        uint32_t id {layers_vector[i]};
        if (p_layer_id != id) {
            continue;
        }

        const Vector3& position = layers_grid_3d.get_position_3d_from_index(i);
        if (!instances_grid_3d.get_data(position)) {
            continue;
        }

        positions.push_back(position);
    }
    
    return positions;
}

#endif

#ifdef DEBUG_ENABLED
void IsometricMap::set_debug_modulate(const Color& p_modulate) const {
    for (const node::IsometricPositionable* positionable : instances_grid_3d.get_internal_array()) {
        positionable->set_debug_modulate(p_modulate);
    }
}
#endif

void IsometricMap::_enter_tree() {
    IsometricPositionable::_enter_tree();

#ifdef TOOLS_ENABLED
    if (last_layer_id == DEFAULT_LAYER_ID && layers.size() == 0) {
        layers[last_layer_id] = DEFAULT_LAYER_NAME;
        set_layers(layers);
    }
#endif

    if (child_positionable_initialized) { return; }
    const Vector<int>& id_vector {grid_3d.get_internal_array()};
    const Vector<uint32_t>& layers_vector {layers_grid_3d.get_internal_array()};
    for (int i = 0; i < id_vector.size(); ++i) {
        add_positionable_as_child(
                id_vector[i],
                grid_3d.get_position_3d_from_index(i),
                layers_vector[i]
        );
    }
    child_positionable_initialized = true;
}

Array IsometricMap::_get_grid_3d() const {
    return grid_3d.to_array();
}

void IsometricMap::_set_grid_3d(const Array& array) {
    grid_3d.from_array(array);
}

Array IsometricMap::_get_layers_grid_3d() const {
    return layers_grid_3d.to_array();
}

void IsometricMap::_set_layers_grid_3d(const Array& array) {
    layers_grid_3d.from_array(array);
}

void IsometricMap::add_positionable_as_child(int positionable_id, const Vector3& p_position, uint32_t layer_id) {
    if (positionable_id == resource::PositionableSet::NONE_POSITIONABLE_ID) { return; }
    if (auto* positionable {
          Object::cast_to<IsometricPositionable>(positionable_set->get_positionable_scene_for_id(positionable_id)->instantiate())}) {
        positionable->set_local_position_3d(p_position);
        positionable->add_to_group(layers_groups[layer_id]);
        add_child(positionable);

#ifdef DEBUG_ENABLED
        Vector<node::IsometricPositionable*> conflicting_instances {
            instances_grid_3d.get_box({p_position, positionable->get_size()})
        };
        bool has_conflict {false};
        for (node::IsometricPositionable* conflict_instance : conflicting_instances) {
            if (!conflict_instance) {
                continue;
            }

            has_conflict = true;
            conflict_instance->set_debug_modulate(node::IsometricPositionable::CONFLICT_MODULATE_COLOR);

            append_conflict_instance(p_position, conflict_instance);
            append_conflict_instance(conflict_instance->get_local_position_3d(), positionable);
        }

        if (has_conflict) { positionable->set_debug_modulate(node::IsometricPositionable::CONFLICT_MODULATE_COLOR);
        }
#endif

        instances_grid_3d.insert_box({p_position, positionable->get_size()}, positionable);

#ifdef TOOLS_ENABLED
        if (!Engine::get_singleton()->is_editor_hint()) return;

        positionable->update_debug_mesh_color(
          get_meta(vformat(LAYER_COLOR_META_NAME_FORMAT, layer_id),Color())
        );
#endif
    }
}

#ifdef DEBUG_ENABLED
void
IsometricMap::append_conflict_instance(const Vector3& p_position, node::IsometricPositionable* p_positionable) {
    if (!conflict_instances_map.has(p_position)) {
        conflict_instances_map[p_position] = Vector<node::IsometricPositionable*>();
    }

    Vector<node::IsometricPositionable*>& conflict_instances_at_position {conflict_instances_map[p_position]};
    if (conflict_instances_at_position.has(p_positionable)) {
        return;
    }
    conflict_instances_at_position.push_back(p_positionable);
}
#endif

void IsometricMap::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_positionable_set"), &IsometricMap::get_positionable_set);
    ClassDB::bind_method(D_METHOD("set_positionable_set"), &IsometricMap::set_positionable_set);
    ClassDB::bind_method(D_METHOD("_get_grid_3d"), &IsometricMap::_get_grid_3d);
    ClassDB::bind_method(D_METHOD("_set_grid_3d"), &IsometricMap::_set_grid_3d);
    ClassDB::bind_method(D_METHOD("_get_layers_grid_3d"), &IsometricMap::_get_layers_grid_3d);
    ClassDB::bind_method(D_METHOD("_set_layers_grid_3d"), &IsometricMap::_set_layers_grid_3d);
    ClassDB::bind_method(D_METHOD("get_layers"), &IsometricMap::get_layers);
    ClassDB::bind_method(D_METHOD("set_layers"), &IsometricMap::set_layers);

    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "positionable_set"), "set_positionable_set", "get_positionable_set");
    ADD_PROPERTY(
        PropertyInfo(Variant::ARRAY, "grid_3d", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR | PROPERTY_USAGE_INTERNAL),
        "_set_grid_3d",
        "_get_grid_3d"
    );
    ADD_PROPERTY(
        PropertyInfo(Variant::ARRAY, "layers_grid_3d", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR | PROPERTY_USAGE_INTERNAL),
        "_set_layers_grid_3d",
        "_get_layers_grid_3d"
    );
    ADD_PROPERTY(
        PropertyInfo(Variant::DICTIONARY, "layers", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR | PROPERTY_USAGE_INTERNAL),
        "set_layers",
        "get_layers"
    );

    ADD_SIGNAL(MethodInfo("positional_set_changed", PropertyInfo(Variant::OBJECT, "set", PROPERTY_HINT_RESOURCE_TYPE, "PositionalSet")));

#ifdef TOOLS_ENABLED
    ClassDB::bind_method(D_METHOD("get_last_layer_id"), &IsometricMap::get_last_layer_id);
    ClassDB::bind_method(D_METHOD("set_last_layer_id"), &IsometricMap::set_last_layer_id);

    ADD_PROPERTY(
        PropertyInfo(Variant::INT, "last_layer_id", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR | PROPERTY_USAGE_INTERNAL),
        "set_last_layer_id",
        "get_last_layer_id"
    );
#endif
}