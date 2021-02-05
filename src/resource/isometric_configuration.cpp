#include "isometric_configuration.h"

using namespace resource;

IsometricConfiguration::IsometricConfiguration() :
		tile_width(DEFAULT_WIDTH), angle(DEFAULT_ANGLE), topological_margin(DEFAULT_MARGIN) {
	tile_height = calculate_tile_height();
	e_z = calculate_ez();
	z_ratio = e_z / static_cast<float>(tile_height);
}

IsometricConfiguration *IsometricConfiguration::getDefaultConfiguration() {
	static IsometricConfiguration singleton;
	return &singleton;
}

int IsometricConfiguration::calculate_tile_height() const {
	return static_cast<int>(round(tile_width * sin(Math::deg2rad(static_cast<double>(angle)))));
}

float IsometricConfiguration::calculate_ez() const {
	return static_cast<float>((tile_width / sqrt(2)) * cos(Math::deg2rad(static_cast<double>(angle))));
}

int IsometricConfiguration::get_tile_width() const {
	return tile_width;
}

void IsometricConfiguration::set_tile_width(int t_w) {
	tile_width = t_w;
	tile_height = calculate_tile_height();
	e_z = calculate_ez();
}

int IsometricConfiguration::get_tile_height() const {
	return tile_height;
}

int IsometricConfiguration::get_angle() const {
	return angle;
}

void IsometricConfiguration::set_angle(int agl) {
	angle = agl;
	tile_height = calculate_tile_height();
	e_z = calculate_ez();
}

float IsometricConfiguration::get_e_z() const {
	return e_z;
}

float IsometricConfiguration::get_z_ratio() const {
	return z_ratio;
}

float IsometricConfiguration::get_topological_margin() const {
	return topological_margin;
}

void IsometricConfiguration::set_topological_margin(float margin) {
	topological_margin = margin;
}

Vector2 IsometricConfiguration::get_screen_coord_from_3d(const Vector3 &pos) const {
	return {
		(pos.x - pos.y) * static_cast<real_t>(tile_width) * 0.5f,
		(pos.x + pos.y) * static_cast<real_t>(tile_height) * 0.5f - e_z * pos.z
	};
}

Vector3 IsometricConfiguration::get_3d_coord_from_screen(const Vector2 &pos, real_t orth_z) const {
	real_t iso_x{ pos.x };
	real_t iso_y{ pos.y };
	auto tile_width_float = static_cast<real_t>(tile_width);
	auto tile_height_float = static_cast<real_t>(tile_height);
	real_t orth_x{ iso_x / tile_width_float + (iso_y + orth_z * e_z) / tile_height_float };
	real_t orth_y{ (iso_y + orth_z * e_z) / tile_height_float - iso_x / tile_width_float };
	return {
		::roundf(orth_x),
		::roundf(orth_y),
		::roundf(orth_z)
	};
}

bool IsometricConfiguration::do_iso_elements_overlap(const data::IsometricPositionableData *data1, const data::IsometricPositionableData *data2) {
	Hexagone hex1 = get_hexagone_2D_coordinates(data1);
	Hexagone hex2 = get_hexagone_2D_coordinates(data2);

	return !(hex1.minX >= hex2.maxX || hex2.minX >= hex1.maxX) &&
		   !(hex1.minY >= hex2.maxY || hex2.minY >= hex1.maxY) &&
		   !(hex1.hMin >= hex2.hMax || hex2.hMin >= hex1.hMax);
}

bool IsometricConfiguration::is_box_in_front(const AABB &box, const AABB &other) {
	const Vector3 &boxEnd{ box.position + box.size };
	const Vector3 &otherEnd{ other.position + other.size };

	if (boxEnd.x - other.position.x <= topological_margin) {
		return false;
	} else if (otherEnd.x - box.position.x <= topological_margin) {
		return true;
	}

	if (boxEnd.y - other.position.y <= topological_margin) {
		return false;
	} else if (otherEnd.y - box.position.y <= topological_margin) {
		return true;
	}

	if (boxEnd.z - other.position.z <= topological_margin) {
		return false;
	} else if (otherEnd.z - box.position.z <= topological_margin) {
		return true;
	}

	const Vector3 &distance{ box.position + boxEnd - other.position - otherEnd };
	const Vector3 &cameraVector{ 1, 1, z_ratio };
	return distance.dot(cameraVector) >= 0;
}

IsometricConfiguration::Hexagone IsometricConfiguration::get_hexagone_2D_coordinates(const data::IsometricPositionableData *data) const {
	const Vector3 &ortho_position{ data->aabb.position };
	const Vector3 &size{ data->aabb.size };
	const Vector3 &upper_point{ Vector3(ortho_position.x, ortho_position.y, ortho_position.z + get_z_ratio() * size.z) };
	const Vector3 &lower_point{ Vector3(ortho_position.x + size.x, ortho_position.y + size.y, ortho_position.z) };
	const Vector3 &left_point{ Vector3(ortho_position.x, ortho_position.y + size.y, ortho_position.z) };
	const Vector3 &right_point{ Vector3(ortho_position.x + size.x, ortho_position.y, ortho_position.z) };

	const real_t minX = upper_point.x - upper_point.z;
	const real_t maxX = lower_point.x - lower_point.z;
	const real_t minY = upper_point.y - upper_point.z;
	const real_t maxY = lower_point.y - lower_point.z;
	const real_t hMin = left_point.x - left_point.y;
	const real_t hMax = right_point.x - right_point.y;

	return { minX, maxX, minY, maxY, hMin, hMax };
}

void IsometricConfiguration::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_angle"), &IsometricConfiguration::get_angle);
	ClassDB::bind_method(D_METHOD("set_angle"), &IsometricConfiguration::set_angle);
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "angle"), "set_angle", "get_angle");
	ADD_PROPERTY_DEFAULT("angle", DEFAULT_ANGLE);

	ClassDB::bind_method(D_METHOD("get_tile_width"), &IsometricConfiguration::get_tile_width);
	ClassDB::bind_method(D_METHOD("set_tile_width"), &IsometricConfiguration::set_tile_width);
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "tile_width"), "set_tile_width", "get_tile_width");
	ADD_PROPERTY_DEFAULT("tile_width", DEFAULT_WIDTH);

	ClassDB::bind_method(D_METHOD("get_margin"), &IsometricConfiguration::get_topological_margin);
	ClassDB::bind_method(D_METHOD("set_margin"), &IsometricConfiguration::set_topological_margin);
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "margin"), "set_margin", "get_margin");
	ADD_PROPERTY_DEFAULT("margin", DEFAULT_MARGIN);
}