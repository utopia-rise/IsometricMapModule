#include "isometric_server.h"

#include "resource/isometric_configuration.h"
#include "servers/rendering_server.h"
#include "utils/isometric_maths.h"

using namespace data;

IsometricServer* IsometricServer::_instance = nullptr;

IsometricServer::IsometricServer() : state {ASYNC_DONE}, exit_thread(false), command_queue(true), is_debug(false) {
    worlds.reserve(8);
    stack.reserve(32);
    thread.start(&IsometricServer::iteration, this);
    RenderingServer::get_singleton()->connect(SNAME("frame_pre_draw"), callable_mp(this, &IsometricServer::fetch_data_and_request_ordering));
}

///////////////STATIC//////////////////

IsometricServer* IsometricServer::get_instance() {
    return _instance;
}

void IsometricServer::create_server() {
    if (!_instance) { _instance = memnew(IsometricServer); }
}

void IsometricServer::iteration(void* p_udata) {
    auto* server {reinterpret_cast<IsometricServer*>(p_udata)};

    while (!server->exit_thread) {
        server->command_queue.wait_and_flush();
    }
    server->command_queue.flush_all();
}

void IsometricServer::terminate_server() {
    if (_instance) {
        _instance->command_queue.push(_instance, &IsometricServer::command_stop_server);
        _instance->thread.wait_to_finish();
        memdelete(_instance);
        _instance = nullptr;
    }
}

///////////PUBLIC API////////////

RID IsometricServer::space_create() {
    data::IsometricSpace* space {memnew(data::IsometricSpace())};
    RID rid {worlds_owner.make_rid(space)};
    command_queue.push(this, &IsometricServer::command_space_create, space);
    return rid;
}

void IsometricServer::space_attach_isometric_element(const RID space_rid, RID element_rid) {
    GET_SPACE_RID_DATA(space, space_rid);
    GET_ELEMENT_RID_DATA(element, element_rid);
    element->space_rid = space_rid;
    command_queue.push(this, &IsometricServer::command_space_attach_isometric_element, space, element);
}

void IsometricServer::space_detach_isometric_element(RID element_rid) {
    GET_ELEMENT_RID_DATA(element, element_rid);
    command_queue.push(this, &IsometricServer::command_space_detach_isometric_element, element);
}

RID IsometricServer::isometric_element_create(bool is_dynamic, AABB aabb) {
    IsometricElement* element {memnew(IsometricElement())};
    element->depth = 1;
    element->aabb = aabb;
    element->is_dynamic = is_dynamic;
    RID rid {elements_owner.make_rid(element)};
    command_queue.push(this, &IsometricServer::command_isometric_element_create, element, is_dynamic);
    return rid;
}

void IsometricServer::isometric_element_attach_canvas_item(const RID element_rid, const RID canvas_rid, const int depth) {
    GET_ELEMENT_RID_DATA(element, element_rid);
    element->visual_rid = canvas_rid;
    command_queue.push(this, &IsometricServer::command_isometric_element_set_depth, element, depth);
    command_queue.push(this, &IsometricServer::command_isometric_element_attach_canvas_item, element, canvas_rid);
}

void IsometricServer::isometric_element_detach_canvas_item(const RID element_rid) {
    GET_ELEMENT_RID_DATA(element, element_rid);
    element->visual_rid = RID();
    command_queue.push(this, &IsometricServer::command_isometric_element_detach_canvas_item, element);
}

void IsometricServer::space_update_configuration(const RID space_rid, RID conf_RID) {
    GET_SPACE_RID_DATA(space, space_rid);
    resource::IsometricConfiguration* conf = resource::IsometricConfiguration::get_instance(conf_RID);
    space->configuration = IsometricParameters(conf->get_tile_width(), conf->get_angle(), conf->get_topological_margin());
}

const data::IsometricParameters* IsometricServer::space_get_configuration(const RID space_rid) {
    GET_SPACE_RID_DATA_WITH_RET(space, space_rid, nullptr);
    return &space->configuration;
}

void IsometricServer::isometric_element_set_position(const RID element_rid, const Vector3 global_position) {
    GET_ELEMENT_RID_DATA(element, element_rid);
    if (worlds_owner.owns(element->space_rid) && !element->is_dynamic) {
        LOG_WARNING(vformat("A static element position can't be changed after "
                            "being attached to a space."));
        return;
    }
    command_queue.push(this, &IsometricServer::command_isometric_element_set_position, element, global_position);
}

void IsometricServer::isometric_element_set_size(const RID element_rid, const Vector3 size) {
    GET_ELEMENT_RID_DATA(element, element_rid);
    if (worlds_owner.owns(element->space_rid) && !element->is_dynamic) {
        LOG_WARNING(vformat("A static element size can't be changed after "
                            "being attached to a space."));
        return;
    }
    command_queue.push(this, &IsometricServer::command_isometric_element_set_size, element, size);
}

void IsometricServer::isometric_element_set_depth(const RID element_rid, const int depth) {
    GET_ELEMENT_RID_DATA(element, element_rid);
    if (worlds_owner.owns(element->space_rid) && !element->is_dynamic) {
        LOG_WARNING(vformat("A static element size can't be changed after "
                            "being attached to a space."));
        return;
    }
    command_queue.push(this, &IsometricServer::command_isometric_element_set_depth, element, depth);
}

int IsometricServer::space_get_diamond_width(const RID space_rid) {
    GET_SPACE_RID_DATA_WITH_RET(space, space_rid, 0);
    return space->configuration.diamond_width;
}

int IsometricServer::space_get_diamond_height(const RID space_rid) {
    GET_SPACE_RID_DATA_WITH_RET(space, space_rid, 0);
    return space->configuration.diamond_height;
}

float IsometricServer::space_get_z_length(const RID space_rid) {
    GET_SPACE_RID_DATA_WITH_RET(space, space_rid, 0.);
    return space->configuration.z_length;
}

void IsometricServer::set_synchronous_mode(bool sync) {
    if (sync) {
        state = SYNC;
    } else {
        state = ASYNC_DONE;
    }
}

void IsometricServer::fetch_data_and_request_ordering() {
    if (state == SYNC) {
        command_queue.push_and_sync(this, &IsometricServer::command_isometric_sort);
        command_queue.push_and_sync(this, &IsometricServer::command_update_visual_server);
    } else if (state == ASYNC_DONE) {
        state = ASYNC_REQUESTED;
        command_queue.push_and_sync(this, &IsometricServer::command_update_visual_server);
        command_queue.push(this, &IsometricServer::command_isometric_sort);
    }
}

void IsometricServer::free_rid(const RID rid) {
    if (worlds_owner.owns(rid)) {
        IsometricSpace* space {worlds_owner.get_or_null(rid)};
        worlds_owner.free(rid);
        command_queue.push(this, &IsometricServer::command_space_delete, space);
        return;
    }
    if (elements_owner.owns(rid)) {
        IsometricElement* element {elements_owner.get_or_null(rid)};
        elements_owner.free(rid);
        command_queue.push(this, &IsometricServer::command_isometric_element_delete, element);
        return;
    }
    LOG_WARNING(vformat("This is not a valid space RID nor a valid isometric element RID: %s", rid.get_id()));
}

void IsometricServer::set_debug(bool p_debug) {
    command_queue.push(this, &IsometricServer::command_set_debug, p_debug);
}

////////COMMANDS/////

void IsometricServer::command_space_create(IsometricSpace* space) {
    worlds.push_back(space);
}

void IsometricServer::command_space_attach_isometric_element(data::IsometricSpace* space, data::IsometricElement* element) {
    Chunk* chunk {get_or_create_chunk(space, element->aabb.position)};

    bool is_dynamic {element->is_dynamic};
    LocalVector<IsometricElement*>& chunk_elements {is_dynamic ? chunk->dynamic_elements : chunk->static_elements};
    chunk_elements.push_back(element);

    LocalVector<IsometricElement*>& elements {is_dynamic ? space->dynamics_elements : space->statics_elements};
    elements.push_back(element);

    space->dirty = true;

    if (is_dynamic) {
        return;
    }

    for (const Chunk* neighbor : chunk->all_neighbors) {
        for (IsometricElement* static_element : neighbor->static_elements) {
            if (utils::are_elements_overlapping(space->configuration, static_element, element)) {
                if (utils::is_box_in_front(space->configuration, static_element->aabb, element->aabb)) {
                    static_element->behind_statics.push_back(element);
                } else {
                    element->behind_statics.push_back(static_element);
                }
            }
        }
    }
}

void IsometricServer::command_space_detach_isometric_element(data::IsometricElement* element) {
    GET_SPACE_RID_DATA(space, element->space_rid);

    space->dirty = true;
    Chunk* chunk {get_or_create_chunk(space, element->aabb.position)};
    if (!element->is_dynamic) {
        space->statics_elements.erase(element);
        for (Chunk* neighbor : chunk->all_neighbors) {
            neighbor->static_elements.erase(element);
            for (IsometricElement* static_element : neighbor->static_elements) {
                static_element->behind_statics.erase(element);
            }
        }
    } else {
        space->dynamics_elements.erase(element);
        chunk->dynamic_elements.erase(element);
    }
    element->behind_statics.clear();
    element->behind_dynamics.clear();
}

void IsometricServer::command_space_delete(IsometricSpace* space) {
    for (IsometricElement* static_element : space->statics_elements) {
        static_element->space_rid = RID();
    }
    space->statics_elements.clear();

    for (IsometricElement* dynamic_element : space->dynamics_elements) {
        dynamic_element->space_rid = RID();
    }
    space->dynamics_elements.clear();
    space->chunks.clear();
    worlds.erase(space);

    memdelete(space);
}

void IsometricServer::command_isometric_element_create(data::IsometricElement* element, bool p_is_dynamic) {}

void IsometricServer::command_isometric_element_attach_canvas_item(data::IsometricElement* element, const RID canvas_rid) {
    if (IsometricSpace* space {worlds_owner.get_or_null(element->space_rid)}) {
        space->dirty = true;
    }
}

void IsometricServer::command_isometric_element_detach_canvas_item(data::IsometricElement* element) {
    if (IsometricSpace* space {worlds_owner.get_or_null(element->space_rid)}) {
        space->dirty = true;
    }
}

void IsometricServer::command_isometric_element_set_position(data::IsometricElement* element, const Vector3 global_position) {
    if (IsometricSpace* space {worlds_owner.get_or_null(element->space_rid)}) {
        space->dirty = true;

        Chunk* former_chunk {get_or_create_chunk(space, element->aabb.position)};
        Chunk* new_chunk {get_or_create_chunk(space, global_position)};

        if (new_chunk != former_chunk) {
            LocalVector<IsometricElement*>& former_chunk_elements {
              element->is_dynamic ? former_chunk->dynamic_elements : former_chunk->static_elements
            };
            former_chunk_elements.erase(element);

            LocalVector<IsometricElement*>& new_chunk_elements {
              element->is_dynamic ? new_chunk->dynamic_elements : new_chunk->static_elements
            };
            new_chunk_elements.push_back(element);
        }
    }

    element->aabb.position = global_position;
}

void IsometricServer::command_isometric_element_set_size(data::IsometricElement* element, const Vector3 size) {
    GET_SPACE_RID_DATA(space, element->space_rid);
    space->dirty = true;

#ifdef DEBUG_ENABLED
    if (static_cast<int>(size.x) > space->cell_size.x || static_cast<int>(size.y) > space->cell_size.y) {
        LOG_WARNING(vformat("Cannot create an isometric element with size on (x, y) > %s", space->cell_size));
    }
#endif
    element->aabb.size = size;
}

void IsometricServer::command_isometric_element_set_depth(data::IsometricElement* element, const int depth) {
    if (IsometricSpace* space {worlds_owner.get_or_null(element->space_rid)}) {
        space->dirty = true;
    }
    element->depth = depth;
}

void IsometricServer::command_isometric_element_delete(data::IsometricElement* element) {
    if (!worlds_owner.owns(element->space_rid)) {
        memdelete(element);
        return;
    }
    IsometricSpace* space {worlds_owner.get_or_null(element->space_rid)};
    space->dirty = true;
    Chunk* chunk {get_or_create_chunk(space, element->aabb.position)};
    if (!element->is_dynamic) {
        space->statics_elements.erase(element);
        for (Chunk* neighbor : chunk->all_neighbors) {
            neighbor->static_elements.erase(element);
            for (IsometricElement* static_element : neighbor->static_elements) {
                static_element->behind_statics.erase(element);
            }
        }
    } else {
        space->dynamics_elements.erase(element);
        chunk->dynamic_elements.erase(element);
    }
    element->behind_statics.clear();
    element->behind_dynamics.clear();
    memdelete(element);
}

void IsometricServer::command_isometric_sort() {
    for (IsometricSpace* world : worlds) {
        if (!world->dirty) { continue; }
        for (IsometricElement* static_element : world->statics_elements) {
            static_element->behind_dynamics.clear();
            static_element->z_order = 0;
            static_element->dirty = true;
            static_element->is_invalid = false;
        }
        for (IsometricElement* dynamic_element : world->dynamics_elements) {
            dynamic_element->behind_dynamics.clear();
            dynamic_element->behind_statics.clear();
            dynamic_element->z_order = 0;
            dynamic_element->dirty = true;
            dynamic_element->is_invalid = false;
        }
        generate_topological_sorting_graph(world);
        world->dirty = false;
        world->fetched = false;
    }

    if (state == ASYNC_REQUESTED) { state = ASYNC_DONE; }
}

void IsometricServer::generate_topological_sorting_graph(data::IsometricSpace* p_isometric_space) {
    for (const KeyValue<Vector2i, Chunk>& key_value : p_isometric_space->chunks) {
        const Chunk& chunk {key_value.value};
        for (const Chunk* neighbor : chunk.dynamic_sorting_neighbors) {
            if (neighbor == &chunk) {
                for (uint32_t i = 0; i < chunk.dynamic_elements.size(); ++i) {
                    IsometricElement * dynamic_element {chunk.dynamic_elements[i]};
                    for (uint32_t j = i + 1; j < neighbor->dynamic_elements.size(); ++j) {
                        IsometricElement* positionable {neighbor->dynamic_elements[j]};
                        if (utils::are_elements_overlapping(p_isometric_space->configuration, dynamic_element, positionable)) {
                            if (utils::is_box_in_front(
                                  p_isometric_space->configuration,
                                  dynamic_element->aabb,
                                  positionable->aabb
                                )) {
                                dynamic_element->behind_dynamics.push_back(positionable);
                            } else {
                                positionable->behind_dynamics.push_back(dynamic_element);
                            }
                        }
                    }
                }
            } else {
                for (IsometricElement* dynamic_element : chunk.dynamic_elements) {
                    for (IsometricElement* positionable : neighbor->dynamic_elements) {
                        if (utils::are_elements_overlapping(p_isometric_space->configuration, dynamic_element, positionable)) {
                            if (utils::is_box_in_front(
                                  p_isometric_space->configuration,
                                  dynamic_element->aabb,
                                  positionable->aabb
                                )) {
                                dynamic_element->behind_dynamics.push_back(positionable);
                            } else {
                                positionable->behind_dynamics.push_back(dynamic_element);
                            }
                        }
                    }
                }
            }

            for (IsometricElement* dynamic_element : chunk.dynamic_elements) {
                for (IsometricElement* static_element : neighbor->static_elements) {
                    if (utils::are_elements_overlapping(p_isometric_space->configuration, dynamic_element, static_element)) {
                        if (utils::is_box_in_front(
                              p_isometric_space->configuration,
                              dynamic_element->aabb,
                              static_element->aabb
                            )) {
                            dynamic_element->behind_statics.push_back(static_element);
                        } else {
                            static_element->behind_dynamics.push_back(dynamic_element);
                        }
                    }
                }
            }
        }
    }

    for (IsometricElement* element : p_isometric_space->statics_elements) {
        if (element->dirty) { sort_isometric_element(element); }
    }

    for (IsometricElement* element : p_isometric_space->dynamics_elements) {
        if (element->dirty) { sort_isometric_element(element); }
    }
}

void IsometricServer::sort_isometric_element(IsometricElement* data) {
    data->dirty = false;
    data->is_invalid = false;
    data->in_stack = true;
    int max_z = 0;
    stack.push_back(data);
    for (IsometricElement* behind : data->behind_statics) {
        if (behind) { max_z = calculate_z_order(behind, max_z); }
    }
    for (IsometricElement* behind : data->behind_dynamics) {
        if (behind) { max_z = calculate_z_order(behind, max_z); }
    }
    data->in_stack = false;
    stack.remove_at(stack.size() - 1);
    data->z_order = max_z;
}

int IsometricServer::calculate_z_order(data::IsometricElement* element_behind, int current_z_order) {
    if (element_behind->dirty) {
        sort_isometric_element(element_behind);
    } else if (element_behind->in_stack) {
        int index = stack.size() - 1;
        IsometricElement* stack_element {stack[index]};
        while (stack_element != element_behind) {
            stack_element->is_invalid = true;
            index--;
            stack_element = stack[index];
        }
        stack_element->is_invalid = true;
    }

    int depth = element_behind->depth;
    int z_order = element_behind->z_order;
    int new_z_order = depth + z_order;
    return new_z_order >= current_z_order ? new_z_order : current_z_order;
}

void IsometricServer::command_update_visual_server() {
    for (IsometricSpace* world : worlds) {
        if (world->fetched) { continue; }

        for (IsometricElement* static_element : world->statics_elements) {
            RID visual_rid {static_element->visual_rid};
            if (!visual_rid.is_valid()) { continue; }
            RenderingServer::get_singleton()->canvas_item_set_z_index(visual_rid, static_element->z_order);
            if (is_debug && static_element->is_invalid) {
                RenderingServer::get_singleton()->canvas_item_set_modulate(visual_rid, Color(1., 0.5, 0.5));
            } else {
                RenderingServer::get_singleton()->canvas_item_set_modulate(
                  visual_rid,
#ifdef TOOLS_ENABLED
                  static_element->debug_modulate
#else
                  Color(1., 1., 1.)
#endif
                );
            }
        }
        for (IsometricElement* dynamic_element : world->dynamics_elements) {
            RID visual_rid {dynamic_element->visual_rid};
            if (!visual_rid.is_valid()) { continue; }
            RenderingServer::get_singleton()->canvas_item_set_z_index(visual_rid, dynamic_element->z_order);
            if (is_debug && dynamic_element->is_invalid) {
                RenderingServer::get_singleton()->canvas_item_set_modulate(visual_rid, Color(1., 0.5, 0.5));
            } else {
                RenderingServer::get_singleton()->canvas_item_set_modulate(
                  visual_rid,
#ifdef TOOLS_ENABLED
                  dynamic_element->debug_modulate
#else
                  Color(1., 1., 1.)
#endif
                );
            }
        }

        world->fetched = true;
    }
}

void IsometricServer::command_set_debug(bool p_debug) {
    is_debug = p_debug;
    for (IsometricSpace* world : worlds) {
        world->fetched = false;
    }
}

void IsometricServer::command_stop_server() {
    exit_thread = true;
}

#ifdef DEBUG_ENABLED
void IsometricServer::isometric_element_set_debug_modulate(const RID element_rid, const Color color) {
    GET_ELEMENT_RID_DATA(element, element_rid);
    command_queue.push(this, &IsometricServer::command_set_debug_modulate, element, color);
}

void IsometricServer::command_set_debug_modulate(data::IsometricElement* element, const Color color) { // NOLINT(*-convert-member-functions-to-static)
    GET_SPACE_RID_DATA(space, element->space_rid);
    space->dirty = true;
    element->debug_modulate = color;
}
#endif

Chunk* IsometricServer::get_or_create_chunk(data::IsometricSpace* space, Vector3 position) {
    Vector2i& cell_size = space->cell_size;
    Vector2i chunk_position {
      static_cast<int>(position.x) / cell_size.x,
      static_cast<int>(position.y) / cell_size.y
    };
    if (Chunk* chunk {space->chunks.getptr(chunk_position)}) {
        return chunk;
    }

    Chunk* chunk = &space->chunks.insert(chunk_position, Chunk())->value;

    chunk->all_neighbors.push_back(chunk);
    chunk->dynamic_sorting_neighbors.push_back(chunk);
    for (int x = -1; x < 2; ++x) {
        for (int y = -1; y < 2; ++y) {
            if (x == 0 && y == 0) {
                continue;
            }

            Vector2i neighbor_position {chunk_position + Vector2i(x, y)};
            if (Chunk* neighbor {space->chunks.getptr(neighbor_position)}) {
                chunk->all_neighbors.push_back(neighbor);
                neighbor->all_neighbors.push_back(chunk);

                if (x == -1 || (x == 0 && y == -1)) {
                    neighbor->dynamic_sorting_neighbors.push_back(chunk);
                } else {
                    chunk->dynamic_sorting_neighbors.push_back(neighbor);
                }
            }
        }
    }

    return chunk;
}

///////BINDING//////

void IsometricServer::_bind_methods() {
    ClassDB::bind_method(D_METHOD("space_create"), &IsometricServer::space_create);
    ClassDB::bind_method(D_METHOD("space_attach_isometric_element"), &IsometricServer::space_attach_isometric_element);
    ClassDB::bind_method(D_METHOD("space_detach_isometric_element"), &IsometricServer::space_detach_isometric_element);
    ClassDB::bind_method(D_METHOD("space_update_configuration"), &IsometricServer::space_update_configuration);
    ClassDB::bind_method(D_METHOD("space_get_diamond_width"), &IsometricServer::space_get_diamond_width);
    ClassDB::bind_method(D_METHOD("space_get_diamond_height"), &IsometricServer::space_get_diamond_height);
    ClassDB::bind_method(D_METHOD("space_get_z_length"), &IsometricServer::space_get_z_length);

    ClassDB::bind_method(D_METHOD("isometric_element_create"), &IsometricServer::isometric_element_create, AABB());
    ClassDB::bind_method(D_METHOD("isometric_element_attach_canvas_item"), &IsometricServer::isometric_element_attach_canvas_item, 1);
    ClassDB::bind_method(D_METHOD("isometric_element_detach_canvas_item"), &IsometricServer::isometric_element_detach_canvas_item);
    ClassDB::bind_method(D_METHOD("isometric_element_set_position"), &IsometricServer::isometric_element_set_position);
    ClassDB::bind_method(D_METHOD("isometric_element_set_size"), &IsometricServer::isometric_element_set_size);
    ClassDB::bind_method(D_METHOD("isometric_element_set_depth"), &IsometricServer::isometric_element_set_depth);

    ClassDB::bind_method(D_METHOD("fetch_data_and_request_ordering"), &IsometricServer::fetch_data_and_request_ordering);
    ClassDB::bind_method(D_METHOD("set_synchronous_mode"), &IsometricServer::set_synchronous_mode);

    ClassDB::bind_method(D_METHOD("free_rid"), &IsometricServer::free_rid);

    ClassDB::bind_method(D_METHOD("set_debug"), &IsometricServer::set_debug);
}
