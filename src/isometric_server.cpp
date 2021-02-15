#include "isometric_server.h"
#include "core/os/os.h"
#include <scene/2d/canvas_item.h>
#include <modules/isometric_maps/src/data/isometric_parameters.h>
#include <modules/isometric_maps/src/resource/isometric_configuration.h>
#include <modules/isometric_maps/src/utils/isometric_maths.h>

using namespace data;

IsometricServer::IsometricServer() {
    thread_exited = false;
    mutex = Mutex::create();
    thread = Thread::create(IsometricServer::iteration, this);
}

IsometricServer* IsometricServer::get_instance() {
    static IsometricServer instance;
    return &instance;
}

void IsometricServer::iteration(void* p_udata) {
    uint64_t msdelay = 3000;

    IsometricServer* server = reinterpret_cast<IsometricServer*>(p_udata);

    while (!server->exit_thread) {
        WARN_PRINT("Loop")
        List<RID> list;
        server->elements_owner.get_owned_list(&list);

        for (int i = 0; i < list.size(); ++i) {
            if (IsometricElement* positionable = server->elements_owner.getornull(list[i])) {
                positionable->behind_dynamics.clear();
                positionable->z_order = 0;
                positionable->dirty = true;
            }
        }

        for (int i = 0; i < server->worlds.size(); ++i) {
            server->generateTopologicalRenderGraph(server->worlds[i]);
        }

        for (int i = 0; i < list.size(); ++i) {
            if (IsometricElement* positionable = server->elements_owner.getornull(list[i])) {
                VisualServer::get_singleton()->canvas_item_set_z_index(positionable->visual_rid, positionable->z_order);
            }
        }

        OS::get_singleton()->delay_usec(msdelay * 1000);
    }


}

RID IsometricServer::create_space() {
    data::IsometricSpace* data{memnew(data::IsometricSpace())};
    worlds.push_back(data);
    return worlds_owner.make_rid(data);
}

void IsometricServer::delete_space(const RID &rid) {
    data::IsometricSpace* data{worlds_owner.getornull(rid)};
    if (!data) {
        WARN_PRINT("THis is not a valid isometric space RID.")
        return;
    }

    worlds.erase(data);
    memdelete(data);
    worlds_owner.free(rid);
}

RID IsometricServer::register_isometric_element(const RID space_rid, RID p_canvas_item, bool p_is_dynamic) {
    data::IsometricSpace* space{worlds_owner.getornull(space_rid)};
    if (!space) {
        WARN_PRINT("THis is not a valid isometric space RID.")
        return RID();
    }

    IsometricElement* isometric_element{memnew(IsometricElement())};
    isometric_element->visual_rid = p_canvas_item;
    isometric_element->is_dynamic = p_is_dynamic;

    if (!p_is_dynamic) {
        space->static_elements.push_back(isometric_element);

        for (int i = 0; i < space->static_elements.size(); i++) {
            IsometricElement* static_element{space->static_elements[i]};
            if (space->configuration.do_iso_elements_overlap(static_element, isometric_element)) {

                if (space->configuration.is_box_in_front(static_element->aabb, isometric_element->aabb)) {
                    static_element->behind_statics.push_back(isometric_element);
                } else {
                    isometric_element->behind_statics.push_back(static_element);
                }
            }
        }

    } else {
        space->dynamic_elements.push_back(isometric_element);
    }

    return elements_owner.make_rid(isometric_element);
}

void IsometricServer::unregister_isometric_element(const RID space_rid, const RID rid) {
    data::IsometricSpace* space{worlds_owner.getornull(space_rid)};
    if (!space) {
        WARN_PRINT("THis is not a valid isometric space RID.")
        return;
    }

    IsometricElement* isometric_element{elements_owner.getornull(rid)};
    if (!isometric_element) {
        WARN_PRINT("THis is not a valid isometric element RID.")
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


void IsometricServer::generateTopologicalRenderGraph(data::IsometricSpace* p_isometric_space) {

    for (int i = 0; i < p_isometric_space->dynamic_elements.size(); ++i) {
        if (IsometricElement* dynamicPositionable{p_isometric_space->dynamic_elements[i]}) {

            for (int j = 0; j < p_isometric_space->dynamic_elements.size(); ++j) {
                IsometricElement* positionable{p_isometric_space->dynamic_elements[i]};
                if (j != i && positionable) {
                    if (p_isometric_space->configuration.do_iso_elements_overlap(dynamicPositionable, positionable)) {

                        if (p_isometric_space->configuration.is_box_in_front(dynamicPositionable->aabb,
                                                                             positionable->aabb)) {
                            dynamicPositionable->behind_dynamics.push_back(positionable);
                        } else {
                            positionable->behind_dynamics.push_back(dynamicPositionable);
                        }
                    }
                }
            }

            for (int j = 0; j < p_isometric_space->static_elements.size(); ++j) {
                if (IsometricElement* positionable = p_isometric_space->dynamic_elements[i]) {
                    if (p_isometric_space->configuration.do_iso_elements_overlap(dynamicPositionable, positionable)) {

                        if (p_isometric_space->configuration.is_box_in_front(dynamicPositionable->aabb,
                                                                             positionable->aabb)) {
                            dynamicPositionable->behind_dynamics.push_back(positionable);
                        } else {
                            positionable->behind_dynamics.push_back(dynamicPositionable);
                        }
                    }
                }
            }
        }
    }

    for (int i = 0; i < p_isometric_space->static_elements.size(); ++i) {
        IsometricElement* positionable = p_isometric_space->static_elements[i];;
        if (positionable && positionable->dirty) {
            render_isometric_element(positionable);
        }
    }
    for (int i = 0; i < p_isometric_space->dynamic_elements.size(); ++i) {
        IsometricElement* positionable{p_isometric_space->dynamic_elements[i]};
        if (positionable && positionable->dirty) {
            render_isometric_element(positionable);
        }
    }
}

void IsometricServer::render_isometric_element(IsometricElement* data) {
    data->dirty = false;
    int maxZ = 0;
    for (int i = 0; i < data->behind_statics.size(); i++) {
        IsometricElement* behind{data->behind_statics[i]};
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
        IsometricElement* behind{data->behind_dynamics[i]};
        if (behind) {
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
        data->z_order = maxZ;
    }
}

void IsometricServer::update_space_configuration(const RID space_rid, const RID conf_rid) {
    data::IsometricSpace* space{worlds_owner.getornull(space_rid)};
    if (!space) {
        WARN_PRINT("THis is not a valid isometric space RID.")
        return;
    }
    resource::IsometricConfiguration* conf = resource::IsometricConfiguration::get_instance(conf_rid);
    space->configuration = IsometricParameters(conf->get_tile_width(), conf->get_angle(), conf->get_topological_margin());
}

data::IsometricParameters* IsometricServer::get_space_configuration_from_element(const RID element_rid) {
    IsometricElement* isometric_element{elements_owner.getornull(element_rid)};
    if (!isometric_element) {
        WARN_PRINT("THis is not a valid isometric element RID.")
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



