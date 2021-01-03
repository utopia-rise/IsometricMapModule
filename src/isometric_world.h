#ifndef ISOMETRIC_MAPS_ISOMETRIC_WORLD_H
#define ISOMETRIC_MAPS_ISOMETRIC_WORLD_H


#include <core/vector.h>
#include <modules/isometric_maps/src/positionable/static_isometric_element.h>
#include <modules/isometric_maps/src/positionable/dynamic_isometric_element.h>

class IsometricPositionable;

class IsometricWorld {

private:
    Vector<positionable::StaticIsometricElement> static_elements;
    Vector<positionable::DynamicIsometricElement> dynamic_elements;

    void render_isometric_element(IsometricPositionable* positionable);

public:
    IsometricWorld() = default;
    ~IsometricWorld() = default;

    void register_isometric_element(IsometricPositionable* positionable);
    void unregister_isometric_element(IsometricPositionable* positionable);

    void generateTopologicalRenderGraph();
};


#endif //ISOMETRIC_MAPS_ISOMETRIC_WORLD_H
