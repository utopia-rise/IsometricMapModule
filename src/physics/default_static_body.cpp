#include "default_static_body.h"

using namespace physics;

DefaultStaticBody::DefaultStaticBody() :
		StaticBody() {
	initialize_shapes();
}

DefaultStaticBody::~DefaultStaticBody() {
	delete_shapes();
}

void DefaultStaticBody::_enter_tree() {
	update_collision_shapes();
}

void DefaultStaticBody::_physics_process(float delta) {
	if (parent && parent->get_has_moved()) {
		const Vector3 &parentPosition{ parent->get_global_position_3d() };
		set_global_transform({ { 1, 0, 0, 0, 1, 0, 0, 0, 1 },
				{ parentPosition.x, parentPosition.z, parentPosition.y } });
		calculate_collision_shape();
		parent->set_has_moved(false);
	}
}

void DefaultStaticBody::calculate_collision_shape() {
	IsometricPositionable::SlopeType slope_type{ parent->get_slope_type() };
	const Vector3 &size = parent->get_size_3d();

	const Vector3 &converted_size{ size.x, size.z, size.y };
	PoolVector3Array pool_vector3_array;
	Vector3 origin_point;
	pool_vector3_array.push_back(origin_point);
	pool_vector3_array.push_back({ origin_point.x, origin_point.y, converted_size.z });
	pool_vector3_array.push_back({ converted_size.x, origin_point.y, origin_point.z });
	pool_vector3_array.push_back({ converted_size.x, origin_point.y, converted_size.z });

	switch (slope_type) {
		case IsometricPositionable::SlopeType::NONE:
			pool_vector3_array.push_back({ origin_point.x, converted_size.y, origin_point.z });
			pool_vector3_array.push_back({ origin_point.x, converted_size.y, converted_size.z });
			pool_vector3_array.push_back({ converted_size.x, converted_size.y, origin_point.z });
			pool_vector3_array.push_back(converted_size);
			break;
		case IsometricPositionable::SlopeType::LEFT:
			pool_vector3_array.push_back({ origin_point.x, converted_size.y, origin_point.z });
			pool_vector3_array.push_back({ origin_point.x, converted_size.y, converted_size.z });
			break;
		case IsometricPositionable::SlopeType::RIGHT:
			pool_vector3_array.push_back({ converted_size.x, converted_size.y, origin_point.z });
			pool_vector3_array.push_back(converted_size);
			break;
		case IsometricPositionable::SlopeType::FORWARD:
			pool_vector3_array.push_back({ origin_point.x, converted_size.y, origin_point.z });
			pool_vector3_array.push_back({ converted_size.x, converted_size.y, origin_point.z });
			break;
		case IsometricPositionable::SlopeType::BACKWARD:
			pool_vector3_array.push_back({ origin_point.x, converted_size.y, converted_size.z });
			pool_vector3_array.push_back(converted_size);
			break;
	}

	shape->set_points(pool_vector3_array);
	collision_shape->set_shape(shape);
}

void DefaultStaticBody::update_collision_shapes() {
	calculate_collision_shape();
	if (!collision_shape->get_parent()) {
		add_child(collision_shape);
	}
	collision_shape->set_owner(this);
}

void DefaultStaticBody::_notification(int notif) {
	switch (notif) {
		case NOTIFICATION_ENTER_TREE:
			_enter_tree();
			break;
		case NOTIFICATION_PHYSICS_PROCESS:
			_physics_process(get_physics_process_delta_time());
			break;
		default:
			break;
	}
}

void DefaultStaticBody::_bind_methods() {
}
