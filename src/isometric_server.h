#ifndef ISOMETRIC_MAPS_ISOMETRIC_SERVER_H
#define ISOMETRIC_MAPS_ISOMETRIC_SERVER_H

#include <core/os/thread.h>
#include "data/isometric_space.h"
#include "data/isometric_element.h"
#include "core/command_queue_mt.h"
#include "core/os/spin_lock.h"

class IsometricServer : public Object {
GDCLASS(IsometricServer, Object)
private:
    bool thread_exited;
    mutable bool exit_thread;
    Thread thread;
    SpinLock spin_lock;
    CommandQueueMT command_queue;

    RID_Owner<data::IsometricSpace> worlds_owner;
    RID_Owner<data::IsometricElement> elements_owner;
    Vector<data::IsometricSpace*> worlds;

    Vector<data::IsometricElement*> stack;

    void render_isometric_element(data::IsometricElement* data);
    int update_z_order(data::IsometricElement* element_behind, int current_z_order);
    static void iteration(void* p_udata);
    static uint64_t get_ms_delay();

    void create_space_impl(data::IsometricSpace* isometric_space);
    void delete_space_impl(const RID rid);

    void set_isometric_element_position_impl(const RID element_rid, const Vector3 global_position);
    void set_isometric_element_size_impl(const RID element_rid, const Vector3 size);
    void register_isometric_element_impl(const RID space_rid, data::IsometricElement* isometric_element, bool p_is_dynamic);
    void unregister_isometric_element_impl(const RID space_rid, const RID rid);

public:
    IsometricServer();

    ~IsometricServer() override;

    IsometricServer(const IsometricServer &) = delete;

    static IsometricServer* get_instance();

    static void _bind_methods();

    /////////////////////////PUBLIC API/////////////////////////////////////
    RID create_space();

    void delete_space(const RID rid);

    void update_space_configuration(const RID space_rid, RID conf_RID);

    const data::IsometricParameters* get_space_configuration(const RID world_rid);

    const data::IsometricParameters* get_space_configuration_from_element(const RID element_rid);

    RID register_isometric_element(const RID space_rid, RID p_canvas_item, bool p_is_dynamic, const AABB& aabb);

    void unregister_isometric_element(const RID space_rid, const RID rid);

    void set_isometric_element_position(const RID element_rid, const Vector3 global_position);

    void set_isometric_element_size(const RID element_rid, const Vector3 size);

    uint64_t get_isometric_z_index(const RID element_rid);

    bool is_element_invalid(const RID element_rid);

    /////////////////////////ENGINE INTERNAL API/////////////////////////////////////
    void generate_topological_render_graph(data::IsometricSpace* p_isometric_space);

    int get_isometric_space_diamond_width(const RID space_rid);

    int get_isometric_space_diamond_height(const RID space_rid);

    float get_isometric_space_z_length(const RID space_rid);

    void synchronize_z_order();
};

#endif //ISOMETRIC_MAPS_ISOMETRIC_SERVER_H
