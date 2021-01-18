#ifndef ISOMETRIC_MAPS_ISOMETRIC_WORLD_H
#define ISOMETRIC_MAPS_ISOMETRIC_WORLD_H


#include <core/vector.h>
#include <modules/isometric_maps/src/positionable/static_isometric_element.h>
#include <modules/isometric_maps/src/positionable/dynamic_isometric_element.h>
#include <modules/isometric_maps/src/isometric_positionable_data.h>

class IsometricWorld {

private:
    Vector<IsometricPositionableData*> static_elements;
    Vector<IsometricPositionableData*> dynamic_elements;

    void render_isometric_element(positionable::IsometricPositionable* positionable);
    void order_statics();

public:
    IsometricWorld();
    ~IsometricWorld() = default;

    void register_isometric_element(IsometricPositionableData* p_positionable_data, bool p_is_dynamic);
    void unregister_isometric_element(IsometricPositionableData* p_positionable_data, bool p_is_dynamic);

    void generateTopologicalRenderGraph();
};


#endif //ISOMETRIC_MAPS_ISOMETRIC_WORLD_H
