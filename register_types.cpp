#include "register_types.h"
#include <core/engine.h>
#include <modules/isometric_maps/src/isometric_api.h>
#include <modules/isometric_maps/src/positionable/static_isometric_element.h>
#include <modules/isometric_maps/src/positionable/isometric_tile.h>
#include <modules/isometric_maps/src/positionable/isometric_placeholder.h>
#include <modules/isometric_maps/src/positionable/dynamic_isometric_element.h>

void register_isometric_maps_types() {
    ClassDB::register_class<positionable::IsometricPositionable>();
    ClassDB::register_class<positionable::IsometricMap>();
    ClassDB::register_class<positionable::StaticIsometricElement>();
    ClassDB::register_class<positionable::IsometricTile>();
    ClassDB::register_class<positionable::IsometricPlaceholder>();
    ClassDB::register_class<positionable::DynamicIsometricElement>();

    ClassDB::register_class<positionable::physics::DefaultStaticBody>();
    ClassDB::register_class<positionable::physics::DefaultKinematicBody>();

    ClassDB::register_class<editor::OutlineDrawer>();

    ClassDB::register_class<IsometricApi>();
    Engine::get_singleton()->add_singleton(Engine::Singleton("HilbertHotel", IsometricApi::get_instance()));
}

void unregister_isometric_maps_types() {

}
