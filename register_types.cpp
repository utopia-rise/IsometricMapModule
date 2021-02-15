#include "register_types.h"
#include <core/engine.h>
#include <modules/isometric_maps/src/isometric_server.h>
#include <modules/isometric_maps/src/node/isometric_map.h>
#include <modules/isometric_maps/src/resource/isometric_configuration.h>


void register_isometric_maps_types() {
    ClassDB::register_class<IsometricServer>();
    Engine::get_singleton()->add_singleton(Engine::Singleton("IsometricServer", reinterpret_cast<Object *>(IsometricServer::get_instance())));

    ClassDB::register_class<resource::IsometricConfiguration>();

    ClassDB::register_class<node::IsometricPositionable>();
    ClassDB::register_class<node::IsometricMap>();

    ClassDB::register_class<editor::OutlineDrawer>();
}

void unregister_isometric_maps_types() {

}
