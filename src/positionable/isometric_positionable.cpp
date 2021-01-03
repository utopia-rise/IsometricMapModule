#include "isometric_positionable.h"

using namespace positionable;

IsometricPositionable::IsometricPositionable() {
    connect("enter_tree", this, "_enter_tree");
    connect("exit_tree", this, "_exit_tree");
    on_resize();
}

void IsometricPositionable::_enter_tree() {

}

void IsometricPositionable::_exit_tree() {

}

Transform2D IsometricPositionable::get_hexagone_coordinates() const {
    return Transform2D();
}

void IsometricPositionable::set_outline_drawer(Color color, real_t lineSize) {

}

Vector3 IsometricPositionable::get_local_3d_position() const {
    return Vector3();
}

void IsometricPositionable::set_local_3d_position(Vector3 p_local) {

}

Vector3 IsometricPositionable::get_global_position_3d() const {
    return Vector3();
}

void IsometricPositionable::set_global_position_3d(Vector3 pos) {

}

AABB IsometricPositionable::get_aabb() {
    return AABB();
}

void IsometricPositionable::set_aabb(AABB ab) {

}

Vector3 IsometricPositionable::get_size_3d() const {
    return Vector3();
}

void IsometricPositionable::set_size_3d(Vector3 s) {

}

int IsometricPositionable::get_z_order_size() const {
    return 0;
}

void IsometricPositionable::set_z_order_size(int size) {

}

bool IsometricPositionable::is_rendered() const {
    return false;
}

void IsometricPositionable::set_rendered(bool isRendered) {

}

bool IsometricPositionable::is_temporary() const {
    return false;
}

void IsometricPositionable::set_temporary(bool temp) {

}

int IsometricPositionable::get_debug_z() const {
    return 0;
}

void IsometricPositionable::set_debug_z(int dZ) {

}

void IsometricPositionable::on_resize() {

}

void IsometricPositionable::on_grid_updated(int stair) {

}

void IsometricPositionable::on_select(bool selected) {

}

bool IsometricPositionable::get_has_moved() const {
    return false;
}

void IsometricPositionable::set_has_moved(bool hm) {

}

void IsometricPositionable::prepare_points() {

}

SlopeType
IsometricPositionable::calculate_slope_offset(Vector2* slopeOffset, real_t tileWidthFloat,
                                              real_t tileHeightFloat, real_t width, real_t depth,
                                              real_t ratio) const {
    return SlopeType::NONE;
}

void IsometricPositionable::_bind_methods() {
    ClassDB::bind_method(D_METHOD("_enter_tree"), &IsometricPositionable::_enter_tree);
    ClassDB::bind_method(D_METHOD("_exit_tree"), &IsometricPositionable::_exit_tree);
}
