#include "isometric_world.h"
#include "isometric_server.h"

using namespace data;

IsometricWorld::IsometricWorld() :
		static_elements(), dynamic_elements() {
}

void IsometricWorld::register_isometric_element(IsometricPositionableData *p_positionable_data, bool p_is_dynamic) {
	if (!p_is_dynamic) {
		static_elements.push_back(p_positionable_data);
		add_static_element(p_positionable_data);
	} else {
		dynamic_elements.push_back(p_positionable_data);
	}
}

void IsometricWorld::unregister_isometric_element(IsometricPositionableData *p_positionable_data) {
	if (!p_positionable_data->is_dynamic) {
		static_elements.erase(p_positionable_data);
		for (int i = 0; i < static_elements.size(); i++) {
			static_elements[i]->behind_statics.erase(p_positionable_data);
		}
	} else {
		dynamic_elements.erase(p_positionable_data);
	}
	p_positionable_data->behind_statics.clear();
	p_positionable_data->behind_dynamics.clear();
}

void IsometricWorld::generateTopologicalRenderGraph() {

	for (int i = 0; i < dynamic_elements.size(); ++i) {
		IsometricPositionableData *dynamicPositionable{ dynamic_elements[i] };
		if (dynamicPositionable) {

			for (int j = 0; j < dynamic_elements.size(); ++j) {
				IsometricPositionableData *positionable{ dynamic_elements[i] };
				if (j != i && positionable ) {
					if (configuration->do_iso_elements_overlap(dynamicPositionable, positionable)) {

						if (configuration->is_box_in_front(dynamicPositionable->aabb, positionable->aabb)) {
							dynamicPositionable->behind_dynamics.push_back(positionable);
						} else {
							positionable->behind_dynamics.push_back(dynamicPositionable);
						}
					}
				}
			}

			for (int j = 0; j < static_elements.size(); ++j) {
				IsometricPositionableData *positionable = dynamic_elements[i];
				if (positionable) {
					if (configuration->do_iso_elements_overlap(dynamicPositionable, positionable)) {

						if (configuration->is_box_in_front(dynamicPositionable->aabb, positionable->aabb)) {
							dynamicPositionable->behind_dynamics.push_back(positionable);
						} else {
							positionable->behind_dynamics.push_back(dynamicPositionable);
						}
					}
				}
			}
		}
	}

	for (int i = 0; i < static_elements.size(); ++i) {
		IsometricPositionableData *positionable = static_elements[i];
		;
		if (positionable && positionable->dirty) {
			render_isometric_element(positionable);
		}
	}
	for (int i = 0; i < dynamic_elements.size(); ++i) {
		IsometricPositionableData *positionable{ dynamic_elements[i] };
		if (positionable && positionable->dirty) {
			render_isometric_element(positionable);
		}
	}
}

void IsometricWorld::render_isometric_element(IsometricPositionableData *data) {
	data->dirty = false;
	int maxZ = 0;
	for (int i = 0; i < data->behind_statics.size(); i++) {
		IsometricPositionableData *behind{ data->behind_statics[i] };
		if (behind) {
			if (behind->dirty) {
				render_isometric_element(behind);
			}
			int zOrderSize = behind->z_size;
			int zOrder = behind->z_order;
			int newZOrder = zOrderSize + zOrder;
			maxZ = newZOrder >= maxZ ? newZOrder : maxZ;
		}
	}
	for (int i = 0; i < data->behind_dynamics.size(); i++) {
		IsometricPositionableData *behind{ data->behind_dynamics[i] };
		if (behind) {
			if (behind) {
				if (behind->dirty) {
					render_isometric_element(behind);
				}
				int zOrderSize = behind->z_size;
				int zOrder = behind->z_order;
				int newZOrder = zOrderSize + zOrder;
				maxZ = newZOrder >= maxZ ? newZOrder : maxZ;
			}
		}
		data->z_order = maxZ;
	}
}

void IsometricWorld::add_static_element(data::IsometricPositionableData *other) {
	for (int i = 0; i < static_elements.size(); i++) {
		IsometricPositionableData *static_element{ static_elements[i] };
		if (configuration->do_iso_elements_overlap(static_element, other)) {

			if (configuration->is_box_in_front(static_element->aabb, other->aabb)) {
				static_element->behind_statics.push_back(other);
			} else {
				other->behind_statics.push_back(static_element);
			}
		}
	}
}
