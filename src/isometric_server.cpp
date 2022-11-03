#include "isometric_server.h"
#include <core/os/os.h>
#include "logging.h"
#include <scene/2d/canvas_item.h>
#include <modules/isometric_maps/src/resource/isometric_configuration.h>
#include <modules/isometric_maps/src/utils/isometric_maths.h>

using namespace data;

IsometricServer* IsometricServer::_instance = nullptr;

IsometricServer::IsometricServer() : ordering_requested(false), exit_thread(false), thread(), spin_lock(), command_queue(true) {
    worlds.reserve(8);
    elements.reserve(512);
    stack.reserve(32);
    thread.start(&IsometricServer::iteration, this);
    VisualServer::get_singleton()->connect("frame_pre_draw", this, "synchronize_z_order");
    VisualServer::get_singleton()->connect("frame_post_draw", this, "request_new_ordering");
}

IsometricServer* IsometricServer::get_instance() {
    return _instance;
}

void IsometricServer::create_server() {
    if(!_instance){
        _instance = memnew(IsometricServer);
    }
}

void IsometricServer::iteration(void* p_udata) {
    auto* server{reinterpret_cast<IsometricServer*>(p_udata)};

    while (!server->exit_thread) {
        server->command_queue.wait_and_flush_one();
    }
    server->command_queue.flush_all();
}

void IsometricServer::terminate_server() {
    if(_instance){
        _instance->command_queue.push(_instance, &IsometricServer::command_stop_server);
        _instance->thread.wait_to_finish();
        memdelete(_instance);
        _instance = nullptr;
    }
}

void IsometricServer::command_stop_server() {
    exit_thread = true;
}

void IsometricServer::request_new_ordering() {
    if(!ordering_requested){
        ordering_requested = true;
        command_queue.push(this, &IsometricServer::command_isometric_sort);
    }
}

void IsometricServer::command_isometric_sort() {
    for (int i = 0; i < elements.size(); i++) {
        if (IsometricElement* positionable{elements[i]}) {
            positionable->behind_dynamics.clear();
            positionable->z_order = 0;
            positionable->dirty = true;
        }
    }

    for (int i = 0; i < worlds.size(); ++i) {
        generate_topological_sorting_graph(worlds[i]);
    }

    SpinLock& lock{spin_lock};
    lock.lock();

    for (int i = 0; i < elements.size(); i++) {
        if (IsometricElement* positionable{elements[i]}) {
            positionable->z_order_update = positionable->z_order;
        }
    }
    lock.unlock();
    ordering_requested = false;
}

void IsometricServer::synchronize_z_order() {
    List<RID> list;
    elements_owner.get_owned_list(&list);

    spin_lock.lock();
    for (List<RID>::Element* current = list.front(); current; current = current->next()) {
        if (IsometricElement* positionable = elements_owner.getornull(current->get())) {
            if (positionable->to_delete) {
                continue;
            }

            RID visual_rid{positionable->visual_rid};
            VisualServer::get_singleton()->canvas_item_set_z_index(visual_rid, positionable->z_order_update);
        }
    }
    spin_lock.unlock();
}

RID IsometricServer::create_space() {
    data::IsometricSpace* data{memnew(data::IsometricSpace())};
    RID rid{worlds_owner.make_rid(data)};
    command_queue.push(this, &IsometricServer::command_create_space, data);
    return rid;
}

void IsometricServer::command_create_space(data::IsometricSpace* isometric_space) {
    worlds.push_back(isometric_space);
}

void IsometricServer::delete_space(const RID rid) {
    command_queue.push(
            this,
            &IsometricServer::command_delete_space,
            rid
    );
}

void IsometricServer::command_delete_space(const RID rid) {
    data::IsometricSpace* data{worlds_owner.getornull(rid)};
    if (!data) {
        WARN_PRINT(vformat("This is not a valid isometric space RID: %s", rid.get_id()));
        return;
    }

    worlds.erase(data);
    memdelete(data);
    worlds_owner.free(rid);
}

RID IsometricServer::register_isometric_element(const RID space_rid, RID p_canvas_item, bool p_is_dynamic,
                                                const AABB& aabb) {
    IsometricElement* isometric_element{memnew(IsometricElement())};
    isometric_element->visual_rid = p_canvas_item;
    isometric_element->is_dynamic = p_is_dynamic;
    isometric_element->world = space_rid;
    isometric_element->aabb = aabb;
    isometric_element->z_size = 1;

    RID rid{elements_owner.make_rid(isometric_element)};
    isometric_element->self = rid;

    command_queue.push(
            this,
            &IsometricServer::command_register_isometric_element,
            space_rid,
            isometric_element,
            p_is_dynamic
    );

    return rid;
}

void
IsometricServer::command_register_isometric_element(const RID space_rid, IsometricElement* isometric_element, bool p_is_dynamic) {
    data::IsometricSpace* space{worlds_owner.getornull(space_rid)};
    if (!space) {
        LOG_WARNING(vformat("This is not a valid isometric space RID: %s", space_rid.get_id()));
    }

    if (!p_is_dynamic) {
        space->static_elements.push_back(isometric_element);

        for (int i = 0; i < space->static_elements.size(); i++) {
            IsometricElement* static_element{space->static_elements[i]};
            if (utils::are_elements_overlapping(space->configuration, static_element, isometric_element)) {

                if (utils::is_box_in_front(space->configuration, static_element->aabb, isometric_element->aabb)) {
                    static_element->behind_statics.push_back(isometric_element);
                } else {
                    isometric_element->behind_statics.push_back(static_element);
                }
            }
        }

    } else {
        space->dynamic_elements.push_back(isometric_element);
    }
    elements.push_back(isometric_element);
}

void IsometricServer::unregister_isometric_element(const RID space_rid, const RID rid) {
    if (IsometricElement* element{elements_owner.getornull(rid)}) {
        element->to_delete = true;
    }
    command_queue.push(this, &IsometricServer::command_unregister_isometric_element, space_rid, rid);
}

void IsometricServer::command_unregister_isometric_element(const RID space_rid, const RID rid) {
    data::IsometricSpace* space{worlds_owner.getornull(space_rid)};
    if (!space) {
        LOG_WARNING(vformat("This is not a valid isometric space RID: %s", space_rid.get_id()));
        return;
    }

    IsometricElement* isometric_element{elements_owner.getornull(rid)};
    if (!isometric_element) {
        LOG_WARNING(vformat("This is not a valid isometric element RID: %s", rid.get_id()));
        return;
    }

    if (!isometric_element->is_dynamic) {
        space->static_elements.erase(isometric_element);
        for (int i = 0; i < space->static_elements.size(); i++) {
            space->static_elements[i]->behind_statics.erase(isometric_element);
        }
    } else {
        space->dynamic_elements.erase(isometric_element);
    }
    isometric_element->behind_statics.clear();
    isometric_element->behind_dynamics.clear();

    elements.erase(isometric_element);
    elements_owner.free(rid);
}

void IsometricServer::generate_topological_sorting_graph(data::IsometricSpace* p_isometric_space) {

    for (int i = 0; i < p_isometric_space->dynamic_elements.size(); ++i) {
        if (IsometricElement* dynamicPositionable{p_isometric_space->dynamic_elements[i]}) {

            for (int j = 0; j < p_isometric_space->dynamic_elements.size(); ++j) {
                IsometricElement* positionable{p_isometric_space->dynamic_elements[i]};
                if (j != i && positionable) {
                    if (utils::are_elements_overlapping(p_isometric_space->configuration, dynamicPositionable,
                                                        positionable)) {

                        if (utils::is_box_in_front(p_isometric_space->configuration, dynamicPositionable->aabb,
                                                   positionable->aabb)) {
                            dynamicPositionable->behind_dynamics.push_back(positionable);
                        } else {
                            positionable->behind_dynamics.push_back(dynamicPositionable);
                        }
                    }
                }
            }

            for (int j = 0; j < p_isometric_space->static_elements.size(); ++j) {
                if (IsometricElement* positionable = p_isometric_space->static_elements[j]) {
                    if (utils::are_elements_overlapping(p_isometric_space->configuration, dynamicPositionable,
                                                        positionable)) {

                        if (utils::is_box_in_front(p_isometric_space->configuration, dynamicPositionable->aabb,
                                                   positionable->aabb)) {
                            dynamicPositionable->behind_statics.push_back(positionable);
                        } else {
                            positionable->behind_dynamics.push_back(dynamicPositionable);
                        }
                    }
                }
            }
        }
    }

    for (int i = 0; i < p_isometric_space->static_elements.size(); ++i) {
        IsometricElement* positionable = p_isometric_space->static_elements[i];
        if (positionable && positionable->dirty) {
            sort_isometric_element(positionable);
        }
    }
    for (int i = 0; i < p_isometric_space->dynamic_elements.size(); ++i) {
        IsometricElement* positionable{p_isometric_space->dynamic_elements[i]};
        if (positionable && positionable->dirty) {
            sort_isometric_element(positionable);
        }
    }
}

void IsometricServer::sort_isometric_element(IsometricElement* data) {
    data->dirty = false;
    data->is_invalid = false;
    int max_z = 0;
    stack.push_back(data);
    for (int i = 0; i < data->behind_statics.size(); i++) {
        if (IsometricElement* behind{data->behind_statics[i]}) {
            max_z = update_z_order(behind, max_z);
        }
    }
    for (int i = 0; i < data->behind_dynamics.size(); i++) {
        if (IsometricElement* behind{data->behind_dynamics[i]}) {
            max_z = update_z_order(behind, max_z);
        }
    }
    stack.remove(stack.size() - 1);
    data->z_order = max_z;
}

int IsometricServer::update_z_order(data::IsometricElement* element_behind, int current_z_order) {
    if (element_behind->dirty) {
        sort_isometric_element(element_behind);
    }
    else {
        int index = stack.size() - 1;
        IsometricElement* stack_element{stack[index]};
        do {
            stack_element->is_invalid = true;
            index--;
            if (index == -1) {
                break;
            }
            stack_element = stack[index];
        } while(stack_element != element_behind);
    }
    int z_order_size = element_behind->z_size;
    int z_order = element_behind->z_order;
    int new_z_order = z_order_size + z_order;
    return new_z_order >= current_z_order ? new_z_order : current_z_order;
}

void IsometricServer::update_space_configuration(const RID space_rid, const RID conf_rid) {
    data::IsometricSpace* space{worlds_owner.getornull(space_rid)};
    if (!space) {
        LOG_WARNING(vformat("This is not a valid space RID: %s", space_rid.get_id()));
        return;
    }
    resource::IsometricConfiguration* conf = resource::IsometricConfiguration::get_instance(conf_rid);
    space->configuration = IsometricParameters(conf->get_tile_width(), conf->get_angle(),
                                               conf->get_topological_margin());
}

const data::IsometricParameters* IsometricServer::get_space_configuration(const RID world_rid) {
    IsometricSpace* world{worlds_owner.getornull(world_rid)};
    if (!world) {
        LOG_WARNING(vformat("This is not a valid space RID: %s", world_rid.get_id()));
        return nullptr;
    }
    return &world->configuration;
}

const data::IsometricParameters* IsometricServer::get_space_configuration_from_element(const RID element_rid) {
    IsometricElement* isometric_element{elements_owner.getornull(element_rid)};
    if (!isometric_element) {
        LOG_WARNING(vformat("This is not a valid isometric element RID: %s", element_rid.get_id()));
        return nullptr;
    }
    return &(worlds_owner.get(isometric_element->world)->configuration);
}

void IsometricServer::set_isometric_element_position(const RID element_rid, const Vector3 global_position) {
    command_queue.push(
            this,
            &IsometricServer::command_set_isometric_element_position,
            element_rid,
            global_position
    );
}

void IsometricServer::command_set_isometric_element_position(const RID element_rid, const Vector3 global_position) {
    IsometricElement* isometric_element{elements_owner.getornull(element_rid)};
    if (!isometric_element) {
        LOG_WARNING(vformat("This is not a valid isometric element RID: %s", element_rid.get_id()));
        return;
    }

    isometric_element->aabb.position = global_position;
}

void IsometricServer::set_isometric_element_size(const RID element_rid, const Vector3 size) {
    command_queue.push(
            this,
            &IsometricServer::command_set_isometric_element_size,
            element_rid,
            size
    );
}

void IsometricServer::command_set_isometric_element_size(const RID element_rid, const Vector3 size) {
    IsometricElement* isometric_element{elements_owner.getornull(element_rid)};
    if (!isometric_element) {
        LOG_WARNING(vformat("This is not a valid isometric element RID: %s", element_rid.get_id()));
        return;
    }

    isometric_element->aabb.size = size;
}

uint64_t IsometricServer::get_isometric_z_index(const RID element_rid) {
    IsometricElement* isometric_element{elements_owner.getornull(element_rid)};
    if (!isometric_element) {
        LOG_WARNING(vformat("This is not a valid isometric element RID: %s", element_rid.get_id()));
        return 0;
    }

    return isometric_element->z_order;
}

bool IsometricServer::is_element_invalid(const RID element_rid) {
    if (IsometricElement* isometric_element{elements_owner.getornull(element_rid)}) {
        return isometric_element->is_invalid;
    }

    LOG_WARNING(vformat("This is not a valid isometric element RID: %s", element_rid.get_id()));
    return false;
}

int IsometricServer::get_isometric_space_diamond_width(const RID space_rid) {
    data::IsometricSpace* space{worlds_owner.getornull(space_rid)};
    if (!space) {
        LOG_WARNING(vformat("This is not a valid space RID: %s", space_rid.get_id()));
        return 0;
    }

    return space->configuration.diamond_width;
}

int IsometricServer::get_isometric_space_diamond_height(const RID space_rid) {
    data::IsometricSpace* space{worlds_owner.getornull(space_rid)};
    if (!space) {
        LOG_WARNING(vformat("This is not a valid space RID: %s", space_rid.get_id()));
        return 0;
    }

    return space->configuration.diamond_height;
}

float IsometricServer::get_isometric_space_z_length(const RID space_rid) {
    data::IsometricSpace* space{worlds_owner.getornull(space_rid)};
    if (!space) {
        LOG_WARNING(vformat("This is not a valid space RID: %s", space_rid.get_id()));
        return 0;
    }

    return space->configuration.z_length;
}

void IsometricServer::_bind_methods() {
    ClassDB::bind_method(D_METHOD("create_space"), &IsometricServer::create_space);
    ClassDB::bind_method(D_METHOD("delete_space"), &IsometricServer::delete_space);

    ClassDB::bind_method(D_METHOD("register_isometric_element"), &IsometricServer::register_isometric_element);
    ClassDB::bind_method(D_METHOD("unregister_isometric_element"), &IsometricServer::unregister_isometric_element);

    ClassDB::bind_method(D_METHOD("update_space_configuration"), &IsometricServer::update_space_configuration);

    ClassDB::bind_method(D_METHOD("synchronize_z_order"), &IsometricServer::synchronize_z_order);
    ClassDB::bind_method(D_METHOD("request_new_ordering"), &IsometricServer::request_new_ordering);
}
