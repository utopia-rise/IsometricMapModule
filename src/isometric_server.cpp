#include "isometric_server.h"

IsometricServer* IsometricServer::get_instance() {
    static IsometricServer instance;
    return &instance;
}

RID IsometricServer::create_world() {
    IsometricWorldData* data{memnew(IsometricWorldData())};
    worlds.push_back(data);
    return worlds_owner.make_rid(data);
}

void IsometricServer::delete_world(const RID& rid) {
    IsometricWorldData* data{worlds_owner.get(rid)};
    worlds.erase(data);
    memdelete(data);
}

RID IsometricServer::register_isometric_element(CanvasItem* p_canvas_item, const RID& world_rid, bool p_is_dynamic) {
    IsometricWorldData* world_data{worlds_owner.get(world_rid)};
    IsometricPositionableData* positionable_data{memnew(IsometricPositionableData())};
    positionable_data->rid = p_canvas_item->get_canvas_item();
    world_data->world->register_isometric_element(positionable_data, p_is_dynamic);
    return positionables_owner.make_rid(positionable_data);
}

void IsometricServer::unregister_isometric_element(const RID& rid, const RID& world_rid, bool p_is_dynamic) {
    IsometricWorldData* world_data{worlds_owner.get(world_rid)};
    IsometricPositionableData* positionable_data{IsometricServer::get_instance()->get_positionables_owner().get(rid)};
    world_data->world->unregister_isometric_element(positionable_data, p_is_dynamic);
}

RID_Owner<IsometricPositionableData>& IsometricServer::get_positionables_owner() {
    return positionables_owner;
}
