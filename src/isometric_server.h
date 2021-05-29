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
    Thread thread;
    Mutex mutex;

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

    /////////////////////////PUBLIC API/////////////////////////////////////
    RID create_space();

    void delete_space(const RID &rid);

    void update_space_configuration(const RID space_rid, RID conf_RID);

    const data::IsometricParameters* get_space_configuration(const RID world_rid);

    const data::IsometricParameters* get_space_configuration_from_element(const RID element_rid);

    RID register_isometric_element(const RID space_rid, RID p_canvas_item, bool p_is_dynamic);

    void unregister_isometric_element(const RID space_rid, const RID rid);

    void set_isometric_element_position(const RID element_rid, const Vector3 global_position);

    void set_isometric_element_size(const RID element_rid, Vector3 size);

    uint64_t get_isometric_z_index(const RID element_rid);


    /////////////////////////ENGINE INTERNAL API/////////////////////////////////////
    void generate_topological_render_graph(data::IsometricSpace* p_isometric_space);

    int get_isometric_space_diamond_width(const RID space_rid);

    int get_isometric_space_diamond_height(const RID space_rid);

    float get_isometric_space_z_length(const RID space_rid);
};

#endif //ISOMETRIC_MAPS_ISOMETRIC_SERVER_H
