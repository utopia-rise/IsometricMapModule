#include "register_types.h"
#include <core/engine.h>
#include <modules/isometric_maps/src/isometric_server.h>
#include <modules/isometric_maps/src/node/isometric_map.h>
#include <modules/isometric_maps/src/positionable/isometric_placeholder.h>
#include <modules/isometric_maps/src/positionable/isometric_tile.h>

void register_isometric_maps_types() {
    ClassDB::register_class<node::IsometricPositionable>();
    ClassDB::register_class<node::IsometricMap>();
    ClassDB::register_class<positionable::IsometricTile>();
    ClassDB::register_class<positionable::IsometricPlaceholder>();


    ClassDB::register_class<editor::OutlineDrawer>();

    ClassDB::register_class<resource::IsometricConfiguration>();
	ClassDB::register_class<IsometricServer>();

    Engine::get_singleton()->add_singleton(Engine::Singleton("IsometricServer", reinterpret_cast<Object *>(IsometricServer::get_instance())));
}

void unregister_isometric_maps_types() {

}
