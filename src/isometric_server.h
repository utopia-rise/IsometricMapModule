#ifndef ISOMETRIC_MAPS_ISOMETRIC_SERVER_H
#define ISOMETRIC_MAPS_ISOMETRIC_SERVER_H

#include "data/isometric_world_data.h"
#include "data/isometric_positionable_data.h"
#include <scene/2d/canvas_item.h>

class IsometricServer : public Object {

private:
	static void iteration(void *p_udata);

public:
	IsometricServer();
	~IsometricServer() = default;
	IsometricServer(const IsometricServer &) = delete;

	static IsometricServer *get_instance();
	static void _bind_methods();

private:
	bool thread_exited;
	mutable bool exit_thread;
	Thread *thread;
	Mutex *mutex;

	RID_Owner<data::IsometricWorldData> worlds_owner;
	RID_Owner<data::IsometricPositionableData> positionables_owner;
	Vector<data::IsometricWorldData *> worlds;

public:
	RID create_world();
	void delete_world(const RID &rid);

	RID register_isometric_element(CanvasItem *p_canvas_item, const RID world_rid, bool p_is_dynamic);
	void unregister_isometric_element(const RID rid, const RID world_rid);

	RID_Owner<data::IsometricPositionableData> &get_isometric_element_world();

};

#endif //ISOMETRIC_MAPS_ISOMETRIC_SERVER_H
