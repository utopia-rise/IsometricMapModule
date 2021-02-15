#ifndef ISOMETRIC_MAPS_ISOMETRIC_SERVER_H
#define ISOMETRIC_MAPS_ISOMETRIC_SERVER_H

#include <core/os/thread.h>
#include "data/isometric_space.h"
#include "data/isometric_element.h"

class IsometricServer : public Object {
GDCLASS(IsometricServer, Object)
private:
    bool thread_exited;
    mutable bool exit_thread;
    Thread* thread;
    Mutex* mutex;

    RID_Owner<data::IsometricSpace> worlds_owner;
    RID_Owner<data::IsometricElement> elements_owner;
    Vector<data::IsometricSpace*> worlds;

    void render_isometric_element(data::IsometricElement* data);

    static void iteration(void* p_udata);

public:
    IsometricServer();

    ~IsometricServer() = default;

    IsometricServer(const IsometricServer &) = delete;

    static IsometricServer* get_instance();

    static void _bind_methods();

private:


public:
    RID create_space();

    void delete_space(const RID &rid);

    RID register_isometric_element(const RID space_rid, RID p_canvas_item, bool p_is_dynamic);

    void unregister_isometric_element(const RID space_rid, const RID rid);

    data::IsometricParameters* get_space_configuration_from_element(const RID element_rid);

    void update_space_configuration(const RID space_rid, RID conf_RID);

    void generateTopologicalRenderGraph(data::IsometricSpace* p_isometric_space);
};

#endif //ISOMETRIC_MAPS_ISOMETRIC_SERVER_H
