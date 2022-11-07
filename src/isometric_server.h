#ifndef ISOMETRIC_MAPS_ISOMETRIC_SERVER_H
#define ISOMETRIC_MAPS_ISOMETRIC_SERVER_H

#include <core/os/thread.h>
#include "data/isometric_space.h"
#include "data/isometric_element.h"
#include "core/command_queue_mt.h"
#include "core/os/spin_lock.h"
#include "logging.h"

#define IS IsometricServer::get_instance()

class IsometricServer : public Object {
GDCLASS(IsometricServer, Object)
private:
    static IsometricServer* _instance;

    mutable bool exit_thread;
    Thread thread;
    CommandQueueMT command_queue;

    SafeNumeric<int> counter;

    enum RequestType{
        SYNC,
        ASYNC_REQUESTED,
        ASYNC_DONE
    };
    RequestType state;
    bool is_debug;

    mutable RID_Owner<data::IsometricSpace> worlds_owner;
    mutable RID_Owner<data::IsometricElement> elements_owner;

    LocalVector<data::IsometricSpace*> worlds;

    LocalVector<data::IsometricElement*> stack;

    static void iteration(void* p_udata);

public:
    IsometricServer();

    ~IsometricServer() = default;

    IsometricServer(const IsometricServer &) = delete;

    static void create_server();
    static IsometricServer* get_instance();
    static void terminate_server();

    static void _bind_methods();

    /////////////////////////PUBLIC API////////////////////////////////////

    ///SPACE
    RID space_create();
    void space_attach_isometric_element(const RID space_rid, RID element_rid);
    void space_update_configuration(const RID space_rid, RID conf_RID);
    const data::IsometricParameters* space_get_configuration(const RID space_rid);
    int space_get_diamond_width(const RID space_rid);
    int space_get_diamond_height(const RID space_rid);
    float space_get_z_length(const RID space_rid);

    ///ISOMETRIC ELEMENT
    RID isometric_element_create(bool is_dynamic, AABB aabb = AABB());
    void isometric_element_attach_canvas_item(const RID element_rid, const RID canvas_rid, const int depth = 1);
    void isometric_element_set_position(const RID element_rid, const Vector3 global_position);
    void isometric_element_set_size(const RID element_rid, const Vector3 size);
    void isometric_element_set_depth(const RID element_rid, const int depth);

    ///ORDERING
    void fetch_data_and_request_ordering();
    void force_synchronous_sort(bool sync);

    ///DELETE
    void free_rid(RID rid);

    ///UTILITIES
    void set_debug(bool p_debug);
private:
     /////////////////////////THREAD COMMANDS/////////////////////////////////////
    void command_space_create(data::IsometricSpace* space);
    void command_space_attach_isometric_element(data::IsometricSpace* space, data::IsometricElement* element);
    void command_space_delete(data::IsometricSpace* space);

    void command_isometric_element_create(data::IsometricElement* element, bool p_is_dynamic);
    void command_isometric_element_attach_canvas_item(data::IsometricElement* element, const RID canvas_rid);
    void command_isometric_element_set_position(data::IsometricElement* element, const Vector3 global_position);
    void command_isometric_element_set_size(data::IsometricElement* element, const Vector3 size);
    void command_isometric_element_set_depth(data::IsometricElement* element, const int depth);
    void command_isometric_element_delete(data::IsometricElement* element);

    void command_isometric_sort();
    void generate_topological_sorting_graph(data::IsometricSpace* p_isometric_space);
    void sort_isometric_element(data::IsometricElement* data);
    _FORCE_INLINE_ int calculate_z_order(data::IsometricElement* element_behind, int current_z_order);

    void command_update_visual_server();
    void command_set_debug(bool p_debug);
    void command_stop_server();

    ///////////////UTILITIES/////////

#define GET_ELEMENT_RID_DATA_WITH_RET(element, element_rid, ret) \
    if(!elements_owner.owns(element_rid)) {\
        LOG_WARNING(vformat("This is not a valid isometric element RID: %s", element_rid.get_id()));\
        return ret;\
    }\
    IsometricElement* element{elements_owner.get(element_rid)};\
    do {} while (false)

#define GET_ELEMENT_RID_DATA(element, element_rid) GET_ELEMENT_RID_DATA_WITH_RET(element, element_rid, void())

#define GET_SPACE_RID_DATA_WITH_RET(space, space_rid, ret) \
    if(!worlds_owner.owns(space_rid)) {\
        LOG_WARNING(vformat("This is not a valid isometric element RID: %s", space_rid.get_id()));\
        return ret;\
    }\
    IsometricSpace* space{worlds_owner.get(space_rid)};\
    do {} while (false)

#define GET_SPACE_RID_DATA(element, element_rid) GET_SPACE_RID_DATA_WITH_RET(element, element_rid, void())
};

#endif //ISOMETRIC_MAPS_ISOMETRIC_SERVER_H
