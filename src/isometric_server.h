#ifndef ISOMETRIC_MAPS_ISOMETRIC_SERVER_H
#define ISOMETRIC_MAPS_ISOMETRIC_SERVER_H


#include "isometric_world_data.h"

class IsometricServer {
private:
    ~IsometricServer() = default;
public:
    IsometricServer() = default;
    IsometricServer(const IsometricServer&) = delete;

    static IsometricServer* get_instance();
private:
    RID_Owner<IsometricWorldData> worlds_owner;
    RID_Owner<IsometricPositionableData> positionables_owner;
    Vector<IsometricWorldData*> worlds;

public:
    RID create_world();
    void delete_world(const RID& rid);

    RID register_isometric_element(CanvasItem* p_canvas_item, const RID& world_rid, bool p_is_dynamic);
    void unregister_isometric_element(const RID& rid, const RID& world_rid, bool p_is_dynamic);

    RID_Owner<IsometricPositionableData>& get_positionables_owner();

};


#endif //ISOMETRIC_MAPS_ISOMETRIC_SERVER_H
