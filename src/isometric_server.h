#ifndef ISOMETRIC_MAPS_ISOMETRIC_SERVER_H
#define ISOMETRIC_MAPS_ISOMETRIC_SERVER_H

#include <core/os/thread.h>
#include "data/isometric_world_data.h"
#include "data/isometric_positionable_data.h"

class IsometricServer : public Object {
GDCLASS(IsometricServer, Object)
private:
    static void iteration(void* p_udata);

public:
    IsometricServer();

    ~IsometricServer() = default;

    IsometricServer(const IsometricServer &) = delete;

    static IsometricServer* get_instance();

    static void _bind_methods();

private:
    bool thread_exited;
    mutable bool exit_thread;
    Thread* thread;
    Mutex* mutex;

    RID_Owner<data::IsometricWorldData> worlds_owner;
    RID_Owner<data::IsometricPositionableData> positionables_owner;
    Vector<data::IsometricWorldData*> worlds;

public:
    RID create_space();

    void delete_space(const RID &rid);

    RID register_isometric_element(const RID world_rid, RID p_canvas_item, bool p_is_dynamic);

    void unregister_isometric_element(const RID world_rid, const RID rid);
};

#endif //ISOMETRIC_MAPS_ISOMETRIC_SERVER_H
