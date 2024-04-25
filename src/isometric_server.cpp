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
    element->space = space;
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
    if (element->space && !element->is_dynamic) {
        LOG_WARNING(vformat("A static element position can't be changed after "
                            "being attached to a space."));
        return;
    }
    command_queue.push(this, &IsometricServer::command_isometric_element_set_position, element, global_position);
}

void IsometricServer::isometric_element_set_size(const RID element_rid, const Vector3 size) {
    GET_ELEMENT_RID_DATA(element, element_rid);
    if (element->space && !element->is_dynamic) {
        LOG_WARNING(vformat("A static element size can't be changed after "
                            "being attached to a space."));
        return;
    }
    command_queue.push(this, &IsometricServer::command_isometric_element_set_size, element, size);
}

void IsometricServer::isometric_element_set_depth(const RID element_rid, const int depth) {
    GET_ELEMENT_RID_DATA(element, element_rid);
    if (element->space && !element->is_dynamic) {
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
    space->dirty = true;

    if (!element->is_dynamic) {
        for (uint32_t i = 0; i < space->static_elements.size(); i++) {
            IsometricElement* static_element {space->static_elements[i]};
            if (utils::are_elements_overlapping(space->configuration, static_element, element)) {
                if (utils::is_box_in_front(space->configuration, static_element->aabb, element->aabb)) {
                    static_element->behind_statics.push_back(element);
                } else {
                    element->behind_statics.push_back(static_element);
                }
            }
        }
        space->static_elements.push_back(element);
    } else {
        space->dynamic_elements.push_back(element);
    }
}

void IsometricServer::command_space_detach_isometric_element(data::IsometricElement* element) {
    IsometricSpace* space {element->space};
    if (space) {
        space->dirty = true;
        if (!element->is_dynamic) {
            space->static_elements.erase(element);
            for (IsometricElement* static_element : space->static_elements) {
                static_element->behind_statics.erase(element);
            }
        } else {
            space->dynamic_elements.erase(element);
        }
        element->behind_statics.clear();
        element->behind_dynamics.clear();
    }
}

void IsometricServer::command_space_delete(IsometricSpace* space) {
    for (IsometricElement* static_element : space->static_elements) {
        static_element->space = nullptr;
    }
    space->static_elements.clear();

    for (IsometricElement* dynamic_element : space->dynamic_elements) {
        dynamic_element->space = nullptr;
    }
    space->dynamic_elements.clear();
    worlds.erase(space);

    memdelete(space);
}

void IsometricServer::command_isometric_element_create(data::IsometricElement* element, bool p_is_dynamic) {}

void IsometricServer::command_isometric_element_attach_canvas_item(data::IsometricElement* element, const RID canvas_rid) {
    IsometricSpace* space {element->space};
    if (space) { element->space->dirty = true; }
}

void IsometricServer::command_isometric_element_detach_canvas_item(data::IsometricElement* element) {
    IsometricSpace* space {element->space};
    if (space) { element->space->dirty = true; }
}

void IsometricServer::command_isometric_element_set_position(data::IsometricElement* element, const Vector3 global_position) {
    IsometricSpace* space {element->space};
    if (space) { element->space->dirty = true; }
    element->aabb.position = global_position;
}

void IsometricServer::command_isometric_element_set_size(data::IsometricElement* element, const Vector3 size) {
    IsometricSpace* space {element->space};
    if (space) { element->space->dirty = true; }
    element->aabb.size = size;
}

void IsometricServer::command_isometric_element_set_depth(data::IsometricElement* element, const int depth) {
    IsometricSpace* space {element->space};
    if (space) { element->space->dirty = true; }
    element->depth = depth;
}

void IsometricServer::command_isometric_element_delete(data::IsometricElement* element) {
    IsometricSpace* space {element->space};
    if (space) {
        space->dirty = true;
        if (!element->is_dynamic) {
            space->static_elements.erase(element);
            for (IsometricElement* static_element : space->static_elements) {
                static_element->behind_statics.erase(element);
            }
        } else {
            space->dynamic_elements.erase(element);
        }
        element->behind_statics.clear();
        element->behind_dynamics.clear();
    }
    memdelete(element);
}

void IsometricServer::command_isometric_sort() {
    for (IsometricSpace* world : worlds) {
        if (!world->dirty) { continue; }
        for (IsometricElement* static_element : world->static_elements) {
            static_element->behind_dynamics.clear();
            static_element->z_order = 0;
            static_element->dirty = true;
            static_element->is_invalid = false;
        }
        for (IsometricElement* dynamic_element : world->dynamic_elements) {
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
    for (uint32_t i = 0; i < p_isometric_space->dynamic_elements.size(); ++i) {
        if (IsometricElement* dynamicPositionable {p_isometric_space->dynamic_elements[i]}) {
            for (uint32_t j = i + 1; j < p_isometric_space->dynamic_elements.size(); ++j) {
                IsometricElement* positionable {p_isometric_space->dynamic_elements[j]};
                if (j != i && positionable) {
                    if (utils::are_elements_overlapping(p_isometric_space->configuration, dynamicPositionable, positionable)) {
                        if (utils::is_box_in_front(
                              p_isometric_space->configuration,
                              dynamicPositionable->aabb,
                              positionable->aabb
                            )) {
                            dynamicPositionable->behind_dynamics.push_back(positionable);
                        } else {
                            positionable->behind_dynamics.push_back(dynamicPositionable);
                        }
                    }
                }
            }

            for (IsometricElement* static_element : p_isometric_space->static_elements) {
                if (static_element) {
                    if (utils::are_elements_overlapping(p_isometric_space->configuration, dynamicPositionable, static_element)) {
                        if (utils::is_box_in_front(
                              p_isometric_space->configuration,
                              dynamicPositionable->aabb,
                              static_element->aabb
                            )) {
                            dynamicPositionable->behind_statics.push_back(static_element);
                        } else {
                            static_element->behind_dynamics.push_back(dynamicPositionable);
                        }
                    }
                }
            }
        }
    }

    for (IsometricElement* static_element : p_isometric_space->static_elements) {
        if (static_element && static_element->dirty) { sort_isometric_element(static_element); }
    }
    for (IsometricElement* dynamic_element : p_isometric_space->dynamic_elements) {
        if (dynamic_element && dynamic_element->dirty) { sort_isometric_element(dynamic_element); }
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

        for (IsometricElement* static_element : world->static_elements) {
            RID visual_rid {static_element->visual_rid};
            if (!visual_rid.is_valid()) { continue; }
            RenderingServer::get_singleton()->canvas_item_set_z_index(visual_rid, static_element->z_order);
            if (is_debug && static_element->is_invalid) {
                RenderingServer::get_singleton()->canvas_item_set_modulate(visual_rid, Color(1., 0.5, 0.5));
            } else {
                RenderingServer::get_singleton()->canvas_item_set_modulate(
                  visual_rid,
#ifdef TOOLS_ENABLED
                  static_element->editor_modulate
#else
                  Color(1., 1., 1.)
#endif
                );
            }
        }
        for (IsometricElement* dynamic_element : world->dynamic_elements) {
            RID visual_rid {dynamic_element->visual_rid};
            if (!visual_rid.is_valid()) { continue; }
            RenderingServer::get_singleton()->canvas_item_set_z_index(visual_rid, dynamic_element->z_order);
            if (is_debug && dynamic_element->is_invalid) {
                RenderingServer::get_singleton()->canvas_item_set_modulate(visual_rid, Color(1., 0.5, 0.5));
            } else {
                RenderingServer::get_singleton()->canvas_item_set_modulate(
                  visual_rid,
#ifdef TOOLS_ENABLED
                  dynamic_element->editor_modulate
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

#ifdef TOOLS_ENABLED
void IsometricServer::isometric_element_set_editor_modulate(const RID element_rid, const Color color) {
    GET_ELEMENT_RID_DATA(element, element_rid);
    command_queue.push(this, &IsometricServer::command_set_editor_modulate, element, color);
}

void IsometricServer::command_set_editor_modulate(data::IsometricElement* element, const Color color) {
    if (IsometricSpace* space {element->space}) { space->dirty = true; }
    element->editor_modulate = color;
}
#endif

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
