#ifndef ISOMETRIC_MAPS_ISOMETRIC_ELEMENT_H
#define ISOMETRIC_MAPS_ISOMETRIC_ELEMENT_H

#include "modules/isometric_maps/src/node/isometric_positionable.h"
#include <scene/3d/physics_body.h>

namespace positionable {
class IsometricElement : public node::IsometricPositionable {
	GDCLASS(IsometricElement, node::IsometricPositionable)
protected:
	bool has_moved;
	bool has_default_body;
	bool is_dynamic;

	/**
         * Sets or remove defaultBody in function of hasDefaultBody
         */
	void update_default_body();

	void _enter_tree() override;

public:
	IsometricElement();
	~IsometricElement() override = default;

	/**
         * @return true if should have default body.
         */
	virtual bool get_has_default_body() const;

	/**
         * Sets if StaticIsometricElement should have default body.
         * @param b
         */
	virtual void set_has_default_body(bool b);

	virtual void update_position_from_body(PhysicsBody *physics_body);

	virtual SlopeType get_slope_type() const;

	void set_aabb(AABB ab) override;
	void set_global_position_3d(Vector3 pos) override;
	void on_resize() override;

	bool get_has_moved() const override;
	void set_has_moved(bool hm) override;
};


IsometricElement::IsometricElement() :
		IsometricPositionable(), has_moved(false), has_default_body(true) {
}


void IsometricElement::_enter_tree() {
	IsometricPositionable::_enter_tree();
}


bool IsometricElement::get_has_default_body() const {
	return has_default_body;
}


void IsometricElement::set_has_default_body(bool b) {
	has_default_body = b;
	if (is_inside_tree()) {
		update_default_body();
	}
}


void IsometricElement::update_position_from_body(PhysicsBody *physics_body) {
	const Vector3 &origin = physics_body->get_global_transform().origin;
	IsometricPositionable::set_global_position_3d({ origin.x, origin.z, origin.y });
}


IsometricElement::SlopeType IsometricElement::get_slope_type() const {
	return SlopeType::NONE;
}


void IsometricElement::set_aabb(AABB ab) {
	IsometricPositionable::set_aabb(ab);
	has_moved = true;
}


void IsometricElement::set_global_position_3d(Vector3 pos) {
	IsometricPositionable::set_global_position_3d(pos);
	has_moved = true;
}

void IsometricElement::on_resize() {
	IsometricPositionable::on_resize();
	has_moved = true;
}


bool IsometricElement::get_has_moved() const {
	return has_moved;
}

void IsometricElement::set_has_moved(bool hm) {
	has_moved = hm;
}
} // namespace positionable

#endif //ISOMETRIC_MAPS_ISOMETRIC_ELEMENT_H
