#include "isometric_server.h"
#include "core/os/os.h"
#include <scene/2d/canvas_item.h>
#include <modules/isometric_maps/src/resource/isometric_configuration.h>
#include <modules/isometric_maps/src/utils/isometric_maths.h>

using namespace data;

IsometricServer::IsometricServer() : thread_exited(false), exit_thread(false), thread(), mutex() {
    thread.start(&IsometricServer::iteration, this);
}

IsometricServer *IsometricServer::get_instance() {
    static IsometricServer instance;
    return &instance;
}

void IsometricServer::iteration(void *p_udata) {
    uint64_t msdelay = 3000;

    auto *server{reinterpret_cast<IsometricServer *>(p_udata)};

    while (!server->exit_thread) {
        List<RID> list;
        server->elements_owner.get_owned_list(&list);

        for (int i = 0; i < list.size(); ++i) {
            if (IsometricElement *positionable = server->elements_owner.getornull(list[i])) {
                positionable->behind_dynamics.clear();
                positionable->z_order = 0;
                positionable->dirty = true;
            }
        }

        for (int i = 0; i < server->worlds.size(); ++i) {
            server->generate_topological_render_graph(server->worlds[i]);
        }

        for (int i = 0; i < list.size(); ++i) {
            if (IsometricElement *positionable = server->elements_owner.getornull(list[i])) {
                VisualServer::get_singleton()->canvas_item_set_z_index(positionable->visual_rid, positionable->z_order);
            }
        }

        OS::get_singleton()->delay_usec(msdelay * 1000);
    }


}

RID IsometricServer::create_space() {
    data::IsometricSpace *data{memnew(data::IsometricSpace())};
    worlds.push_back(data);
    return worlds_owner.make_rid(data);
}

void IsometricServer::delete_space(const RID &rid) {
    data::IsometricSpace *data{worlds_owner.getornull(rid)};
    if (!data) {
        WARN_PRINT(vformat("This is not a valid isometric space RID: %s", rid.get_id()))
        return;
    }

    worlds.erase(data);
    memdelete(data);
    worlds_owner.free(rid);
}

RID IsometricServer::register_isometric_element(const RID space_rid, RID p_canvas_item, bool p_is_dynamic) {
    data::IsometricSpace *space{worlds_owner.getornull(space_rid)};
    if (!space) {
        WARN_PRINT(vformat("This is not a valid isometric space RID: %s", space_rid.get_id()))
        return {};
    }

    IsometricElement *new_isometric_element{memnew(IsometricElement())};
    new_isometric_element->visual_rid = p_canvas_item;
    new_isometric_element->is_dynamic = p_is_dynamic;
    new_isometric_element->world = space_rid;
    new_isometric_element->z_size = 1;

    if (!p_is_dynamic) {
        space->static_elements.push_back(new_isometric_element);

        for (int i = 0; i < space->static_elements.size(); i++) {
            IsometricElement *static_element{space->static_elements[i]};
            if (utils::are_elements_overlapping(space->configuration, static_element, new_isometric_element)) {

                int is_in_front_of = utils::is_box_in_front(space->configuration,
                                                            new_isometric_element->aabb,
                                                            static_element->aabb
                                                            );
                if (is_in_front_of > 0) {
                    new_isometric_element->behind_statics.push_back(static_element);
                } else if (is_in_front_of < 0) {
                    static_element->behind_statics.push_back(new_isometric_element);
                } else {
                    new_isometric_element->behind_statics.push_back(static_element);
                    new_isometric_element->colliding_statics.push_back(static_element);
                    static_element->colliding_statics.push_back(new_isometric_element);
                }
            }
        }

    } else {
        space->dynamic_elements.push_back(new_isometric_element);
    }

    RID rid = elements_owner.make_rid(new_isometric_element);
    new_isometric_element->self = rid;
    return rid;
}

void IsometricServer::unregister_isometric_element(const RID space_rid, const RID rid) {
    data::IsometricSpace *space{worlds_owner.getornull(space_rid)};
    if (!space) {
        WARN_PRINT(vformat("This is not a valid isometric space RID: %s", space_rid.get_id()))
        return;
    }

    IsometricElement *isometric_element{elements_owner.getornull(rid)};
    if (!isometric_element) {
        WARN_PRINT(vformat("This is not a valid isometric element RID: %s", rid.get_id()))
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

    elements_owner.free(rid);
}


void IsometricServer::generate_topological_render_graph(data::IsometricSpace *p_isometric_space) {

    for (int i = 0; i < p_isometric_space->dynamic_elements.size(); ++i) {
        if (IsometricElement *isometric_element{p_isometric_space->dynamic_elements[i]}) {

            for (int j = 0; j < p_isometric_space->dynamic_elements.size(); ++j) {
                IsometricElement *dynamic_element{p_isometric_space->dynamic_elements[i]};
                if (j != i && dynamic_element) {
                    if (utils::are_elements_overlapping(p_isometric_space->configuration, isometric_element,
                                                        dynamic_element)) {

                        int is_in_front_of = utils::is_box_in_front(p_isometric_space->configuration,
                                                                    isometric_element->aabb,
                                                                    dynamic_element->aabb
                        );
                        if (is_in_front_of >= 0) {
                            isometric_element->behind_dynamics.push_back(dynamic_element);
                        } else if (is_in_front_of < 0) {
                            dynamic_element->behind_dynamics.push_back(isometric_element);
                        }
                    }
                }
            }

            for (int j = 0; j < p_isometric_space->static_elements.size(); ++j) {
                if (IsometricElement *static_element = p_isometric_space->dynamic_elements[i]) {
                    if (utils::are_elements_overlapping(p_isometric_space->configuration, isometric_element,
                                                        static_element)) {

                        int is_in_front_of = utils::is_box_in_front(p_isometric_space->configuration,
                                                                    isometric_element->aabb,
                                                                    static_element->aabb
                        );
                        if (is_in_front_of >= 0) {
                            isometric_element->behind_dynamics.push_back(isometric_element);
                        } else if (is_in_front_of < 0) {
                            isometric_element->behind_dynamics.push_back(isometric_element);
                        }
                    }
                }
            }
        }
    }

    for (int i = 0; i < p_isometric_space->static_elements.size(); ++i) {
        IsometricElement *isometric_element = p_isometric_space->static_elements[i];
        if (isometric_element && isometric_element->dirty) {
            render_isometric_element(isometric_element);
        }
    }
    for (int i = 0; i < p_isometric_space->dynamic_elements.size(); ++i) {
        IsometricElement *isometric_element{p_isometric_space->dynamic_elements[i]};
        if (isometric_element && isometric_element->dirty) {
            render_isometric_element(isometric_element);
        }
    }
}

void IsometricServer::render_isometric_element(IsometricElement *data) {
    data->dirty = false;
    int maxZ = 0;
    for (int i = 0; i < data->behind_statics.size(); i++) {
        IsometricElement *behind{data->behind_statics[i]};
        if (behind) {
            if (behind->dirty) {
                render_isometric_element(behind);
            }
            int zOrderSize = behind->z_size;
            int zOrder = behind->z_order;
            int newZOrder = zOrderSize + zOrder;
            maxZ = newZOrder >= maxZ ? newZOrder : maxZ;
        }
    }
    for (int i = 0; i < data->behind_dynamics.size(); i++) {
        IsometricElement *behind{data->behind_dynamics[i]};
        if (behind) {
            if (behind->dirty) {
                render_isometric_element(behind);
            }
            int zOrderSize = behind->z_size;
            int zOrder = behind->z_order;
            int newZOrder = zOrderSize + zOrder;
            maxZ = newZOrder >= maxZ ? newZOrder : maxZ;
        }
    }
    WARN_PRINT(vformat("RID: %s, Zorder %s", data->get_id(), data->z_order))
    data->z_order = maxZ;

}

void IsometricServer::update_space_configuration(const RID space_rid, const RID conf_rid) {
    data::IsometricSpace *space{worlds_owner.getornull(space_rid)};
    if (!space) {
        WARN_PRINT(vformat("This is not a valid space RID: %s", space_rid.get_id()))
        return;
    }
    resource::IsometricConfiguration *conf = resource::IsometricConfiguration::get_instance(conf_rid);
    space->configuration = IsometricParameters(conf->get_tile_width(), conf->get_angle(),
                                               conf->get_topological_margin());
}

const data::IsometricParameters *IsometricServer::get_space_configuration(const RID world_rid) {
    IsometricSpace *world{worlds_owner.getornull(world_rid)};
    if (!world) {
        WARN_PRINT(vformat("This is not a valid space RID: %s", world_rid.get_id()))
        return nullptr;
    }
    return &world->configuration;
}

const data::IsometricParameters *IsometricServer::get_space_configuration_from_element(const RID element_rid) {
    IsometricElement *isometric_element{elements_owner.getornull(element_rid)};
    if (!isometric_element) {
        WARN_PRINT(vformat("This is not a valid isometric element RID: %s", element_rid.get_id()))
        return nullptr;
    }
    return &(worlds_owner.get(isometric_element->world)->configuration);
}


void IsometricServer::_bind_methods() {
    ClassDB::bind_method(D_METHOD("create_space"), &IsometricServer::create_space);
    ClassDB::bind_method(D_METHOD("delete_space"), &IsometricServer::delete_space);

    ClassDB::bind_method(D_METHOD("register_isometric_element"), &IsometricServer::register_isometric_element);
    ClassDB::bind_method(D_METHOD("unregister_isometric_element"), &IsometricServer::unregister_isometric_element);

    ClassDB::bind_method(D_METHOD("update_space_configuration"), &IsometricServer::update_space_configuration);
}

void IsometricServer::set_isometric_element_position(const RID element_rid, const Vector3 global_position) {
    IsometricElement *isometric_element{elements_owner.getornull(element_rid)};
    if (!isometric_element) {
        WARN_PRINT(vformat("This is not a valid isometric element RID: %s", element_rid.get_id()))
        return;
    }

    isometric_element->aabb.position = global_position;
}

void IsometricServer::set_isometric_element_size(const RID element_rid, Vector3 size) {
    IsometricElement *isometric_element{elements_owner.getornull(element_rid)};
    if (!isometric_element) {
        WARN_PRINT(vformat("This is not a valid isometric element RID: %s", element_rid.get_id()))
        return;
    }

    const data::IsometricParameters* params = IsometricServer::get_instance()->get_space_configuration(isometric_element->world);

    if(params){
        float min_size = params->topological_margin * 2;

        if(size.x <= min_size){
            size.x = min_size;
        }
        if(size.y <= min_size){
            size.y = min_size;
        }
        if(size.z <= min_size){
            size.z = min_size;
        }
    }

    isometric_element->aabb.size = size;
}

uint64_t IsometricServer::get_isometric_z_index(const RID element_rid) {
    IsometricElement *isometric_element{elements_owner.getornull(element_rid)};
    if (!isometric_element) {
        WARN_PRINT(vformat("This is not a valid isometric element RID: %s", element_rid.get_id()))
        return 0;
    }

    return isometric_element->z_order;
}

int IsometricServer::get_isometric_space_diamond_width(const RID space_rid) {
    data::IsometricSpace *space{worlds_owner.getornull(space_rid)};
    if (!space) {
        WARN_PRINT(vformat("This is not a valid space RID: %s", space_rid.get_id()))
        return 0;
    }

    return space->configuration.diamond_width;
}

int IsometricServer::get_isometric_space_diamond_height(const RID space_rid) {
    data::IsometricSpace *space{worlds_owner.getornull(space_rid)};
    if (!space) {
        WARN_PRINT(vformat("This is not a valid space RID: %s", space_rid.get_id()))
        return 0;
    }

    return space->configuration.diamond_height;
}

float IsometricServer::get_isometric_space_z_length(const RID space_rid) {
    data::IsometricSpace *space{worlds_owner.getornull(space_rid)};
    if (!space) {
        WARN_PRINT(vformat("This is not a valid space RID: %s", space_rid.get_id()))
        return 0;
    }

    return space->configuration.z_length;
}
