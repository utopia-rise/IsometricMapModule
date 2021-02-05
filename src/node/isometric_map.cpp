#include "isometric_map.h"
#include <modules/isometric_maps/src/resource/isometric_configuration.h>
#include <modules/isometric_maps/src/isometric_world.h>

using namespace node;

IsometricMap::IsometricMap() :
		IsometricPositionable(), draw_tiles(true), tile_width(0), tile_height(0), angle(0), e_z(0), z_ratio(0), topological_margin(0) {
}

void IsometricMap::_ready() {
	for (int i = 0; i < get_child_count(); i++) {
		auto *positionable = cast_to<IsometricPositionable>(get_child(i));
		if (positionable) {
			grid_3d.set_data(positionable->get_local_position_3d(), positionable);
			edition_grid_3d.insert_box(positionable->get_aabb(), positionable);
		}
	}
}

void IsometricMap::add_iso_positionable(IsometricPositionable *isometric_positionable) {
	const Vector3 &map_size{ get_aabb().size };
	const AABB &aabb{ isometric_positionable->get_aabb() };
	const Vector3 &pos{ aabb.position };
	bool is_overlapping;
	auto *child_map{ cast_to<IsometricMap>(isometric_positionable) };
	if (child_map) {
		is_overlapping = are_map_elements_overlapping(pos, child_map);
	} else {
		is_overlapping = edition_grid_3d.is_overlapping(aabb);
	}
	if (pos.x >= map_size.x || pos.y >= map_size.y || pos.z >= map_size.z || is_overlapping) return;
	isometric_positionable->set_is_temporary(false);
	isometric_positionable->set_debug_z(0);

	grid_3d.set_data(aabb.position, isometric_positionable);

	if (child_map) {
		insert_map_as_flatten(child_map, pos);
	} else {
		edition_grid_3d.insert_box(aabb, isometric_positionable);
	}

	add_child(isometric_positionable);
	isometric_positionable->add_to_group(ISO_GROUP, false);
}

void IsometricMap::remove_iso_positionable(IsometricPositionable *isometric_positionable) {
	const Vector3 &map_size{ get_aabb().size };
	const AABB &aabb{ isometric_positionable->get_aabb() };
	const Vector3 &pos{ aabb.position };
	if (pos.x >= map_size.x || pos.y >= map_size.y || pos.z >= map_size.z) return;
	remove_child(isometric_positionable);

	grid_3d.set_data(aabb.position, nullptr);

	if (auto *child_map{ cast_to<IsometricMap>(isometric_positionable) }) {
		const Array &child_map_children{ child_map->get_flatten_positionables(pos) };
		for (int i = 0; i < child_map_children.size(); i++) {
			if (auto *posi{ cast_to<IsometricPositionable>(child_map_children[i]) }) {
				edition_grid_3d.insert_box(posi->get_aabb(), child_map, true);
			}
		}
	} else {
		edition_grid_3d.insert_box(aabb, isometric_positionable, true);
	}
	if (isometric_positionable->is_in_group(ISO_GROUP)) {
		isometric_positionable->remove_from_group(ISO_GROUP);
	}
	isometric_positionable->update();
}

IsometricPositionable *IsometricMap::get_positionable_at(Vector3 pos, bool only_left_upper_corner) {
	return cast_to<IsometricPositionable>(only_left_upper_corner ? grid_3d.get_data(pos) : edition_grid_3d.get_data(pos));
}

bool IsometricMap::is_overlapping(IsometricPositionable *positionable) {
	return edition_grid_3d.is_overlapping(positionable->get_aabb());
}

bool IsometricMap::is_overlapping_aabb(AABB aabb) {
	return edition_grid_3d.is_overlapping(aabb);
}

bool IsometricMap::are_map_elements_overlapping(Vector3 position, IsometricMap *map) {
	for (int i = 0; i < map->get_child_count(); i++) {
		if (auto *positionable{ cast_to<IsometricPositionable>(map->get_child(i)) }) {
			if (auto *child_map{ cast_to<IsometricMap>(positionable) }) {
				if (are_map_elements_overlapping(position + child_map->get_local_position_3d(), child_map)) {
					return true;
				}
			} else {
				if (is_overlapping_aabb({ position + positionable->get_local_position_3d(), positionable->get_size_3d() })) {
					return true;
				}
			}
		}
	}
	return false;
}

bool IsometricMap::has(IsometricPositionable *isometric_positionable) {
	return grid_3d.has(isometric_positionable);
}

Array IsometricMap::get_positionable_children() const {
	Array positionable_children;
	const Vector<Object *> &grid_array = grid_3d.get_internal_array();
	for (int i = 0; i < grid_array.size(); i++) {
		const Variant &element = grid_array[i];
		if (element) {
			positionable_children.append(element);
		}
	}
	return positionable_children;
}

void IsometricMap::on_resize() {
	IsometricPositionable::on_resize();
	grid_3d.update_array_size(get_size_3d());
	edition_grid_3d.update_array_size(get_size_3d());
}

void IsometricMap::on_grid_updated(int stair) {
	for (int i = 0; i < get_child_count(); i++) {
		auto *isometric_positionable = cast_to<IsometricPositionable>(get_child(i));
		if (isometric_positionable) {
			isometric_positionable->on_grid_updated(stair - static_cast<int>(get_local_position_3d().z));
		}
	}
}

void IsometricMap::set_aabb(AABB ab) {
	IsometricPositionable::set_aabb(ab);
	set_has_moved(true);
}

void IsometricMap::set_has_moved(bool hm) {
	for (int i = 0; i < get_child_count(); i++) {
		auto *positionable = cast_to<IsometricPositionable>(get_child(i));
		if (positionable) {
			positionable->set_has_moved(true);
		}
	}
}

Array IsometricMap::get_flatten_positionables(const Vector3 &offset) {
	Array positionables;
	for (int i = 0; i < get_child_count(); i++) {
		auto *positionable = cast_to<IsometricPositionable>(get_child(i));
		if (positionable) {
			if (auto *map = cast_to<IsometricMap>(positionable)) {
				const Array &inner_positionables{ map->get_flatten_positionables(offset + map->get_local_position_3d()) };
				for (int j = 0; j < inner_positionables.size(); j++) {
					if (auto *inner_positionable{ cast_to<IsometricPositionable>(inner_positionables[j]) }) {
						positionables.append(inner_positionable);
					}
				}
			} else {
				auto *duplicate_positionable = cast_to<IsometricPositionable>(positionable->duplicate());
				duplicate_positionable->set_local_position_3d(duplicate_positionable->get_local_position_3d() + offset);
				positionables.append(duplicate_positionable);
			}
		}
	}
	return positionables;
}

void IsometricMap::insert_map_as_flatten(IsometricMap *map, const Vector3 &offset) {
	for (int i = 0; i < get_child_count(); i++) {
		if (auto *positionable{ cast_to<IsometricPositionable>(get_child(i)) }) {
			if (auto *m{ cast_to<IsometricMap>(positionable) }) {
				insert_map_as_flatten(m, offset + m->get_local_position_3d());
			} else {
				const AABB &aabb{ positionable->get_local_position_3d() + offset, positionable->get_size_3d() };
				edition_grid_3d.insert_box(aabb, map);
			}
		}
	}
}

void IsometricMap::_bind_methods() {
	ClassDB::bind_method(D_METHOD("add_iso_positionable"), &IsometricMap::add_iso_positionable);
	ClassDB::bind_method(D_METHOD("remove_iso_positionable"), &IsometricMap::remove_iso_positionable);
	ClassDB::bind_method(D_METHOD("get_positionable_at"), &IsometricMap::get_positionable_at);
	ClassDB::bind_method(D_METHOD("is_overlapping"), &IsometricMap::is_overlapping);
	ClassDB::bind_method(D_METHOD("is_overlapping_aabb"), &IsometricMap::is_overlapping_aabb);
	ClassDB::bind_method(D_METHOD("are_map_elements_overlapping"), &IsometricMap::are_map_elements_overlapping);
	ClassDB::bind_method(D_METHOD("has", "isometric_positionable"), &IsometricMap::has);
	ClassDB::bind_method(D_METHOD("get_positionable_children"), &IsometricMap::get_positionable_children);
}

void IsometricMap::_notification(int notif) {
	switch (notif) {
		case NOTIFICATION_READY:
			_ready();
			break;
		case NOTIFICATION_PROCESS:
			_process(get_process_delta_time());
			break;
		default:
			break;
	}
}
