#include "isometric_server.h"
#include "core/os/os.h"
#include <scene/2d/canvas_item.h>

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
        server->positionables_owner.get_owned_list(&list);

        for (int i = 0; i < list.size(); ++i) {
            if (IsometricPositionableData* positionable = server->positionables_owner.getornull(list[i])) {
                positionable->behind_dynamics.clear();
                positionable->z_order = 0;
                positionable->dirty = true;
            }
        }

        for (int i = 0; i < server->worlds.size(); ++i) {
            server->worlds[i]->world.generateTopologicalRenderGraph();
        }

        for (int i = 0; i < list.size(); ++i) {
            if (IsometricPositionableData* positionable = server->positionables_owner.getornull(list[i])) {
                VisualServer::get_singleton()->canvas_item_set_z_index(positionable->visual_rid, positionable->z_order);
            }
        }

        OS::get_singleton()->delay_usec(msdelay * 1000);
    }


}

RID IsometricServer::create_space() {
    IsometricWorldData* data{memnew(IsometricWorldData())};
    worlds.push_back(data);
    return worlds_owner.make_rid(data);
}

void IsometricServer::delete_space(const RID &rid) {
    IsometricWorldData* data{worlds_owner.getornull(rid)};
    if(!data){
        WARN_PRINT("THis is not a valid isometric space RID.")
        return;
    }

    worlds.erase(data);
    memdelete(data);
    worlds_owner.free(rid);
}

RID IsometricServer::register_isometric_element(const RID world_rid, RID p_canvas_item, bool p_is_dynamic) {
    IsometricWorldData* world_data{worlds_owner.getornull(world_rid)};
    if(!world_data){
        WARN_PRINT("THis is not a valid isometric space RID.")
        return RID();
    }

    IsometricPositionableData* positionable_data{memnew(IsometricPositionableData())};
    positionable_data->visual_rid = p_canvas_item;
    positionable_data->is_dynamic = p_is_dynamic;

    world_data->world.register_isometric_element(positionable_data, p_is_dynamic);
    return positionables_owner.make_rid(positionable_data);
}

void IsometricServer::unregister_isometric_element(const RID world_rid, const RID rid) {
    IsometricWorldData* world_data{worlds_owner.get(world_rid)};
    if(!world_data){
        WARN_PRINT("THis is not a valid isometric space RID.")
        return;
    }

    IsometricPositionableData* positionable_data{positionables_owner.getornull(rid)};
    world_data->world.unregister_isometric_element(positionable_data);
    positionables_owner.free(rid);
}

void IsometricServer::_bind_methods() {
    ClassDB::bind_method(D_METHOD("create_space"), &IsometricServer::create_space);
    ClassDB::bind_method(D_METHOD("delete_space"), &IsometricServer::delete_space);

    ClassDB::bind_method(D_METHOD("register_isometric_element"), &IsometricServer::register_isometric_element);
    ClassDB::bind_method(D_METHOD("unregister_isometric_element"), &IsometricServer::unregister_isometric_element);
}
