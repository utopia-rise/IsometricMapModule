#ifndef ISOMETRIC_MAPS_ISOMETRIC_WORLD_H
#define ISOMETRIC_MAPS_ISOMETRIC_WORLD_H

#include "resource/isometric_configuration.h"
#include <core/vector.h>
#include "data/isometric_positionable_data.h"

class IsometricWorld {

private:
	resource::IsometricConfiguration *configuration;
	Vector<data::IsometricPositionableData *> static_elements;
	Vector<data::IsometricPositionableData *> dynamic_elements;

	void render_isometric_element(data::IsometricPositionableData *data);
	void add_static_element(data::IsometricPositionableData *other);

public:
	IsometricWorld();
	~IsometricWorld() = default;

	void register_isometric_element(data::IsometricPositionableData *p_positionable_data, bool p_is_dynamic);
	void unregister_isometric_element(data::IsometricPositionableData *p_positionable_data);

	void generateTopologicalRenderGraph();
};

#endif //ISOMETRIC_MAPS_ISOMETRIC_WORLD_H
