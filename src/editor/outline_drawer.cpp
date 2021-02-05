#include "outline_drawer.h"

using namespace editor;

OutlineDrawer::OutlineDrawer() :
		Node2D(), up_points(nullptr), down_points(nullptr), line_size(1) {
	connect("draw", this, "_draw");

	set_z_index(4096);
}

void OutlineDrawer::_draw() {
	//    Upper Lines
	Vector<Vector2> up_p = *up_points;
	draw_line(up_p[0], up_p[1], color, line_size);
	draw_line(up_p[1], up_p[2], color, line_size);
	draw_line(up_p[2], up_p[3], color, line_size);
	draw_line(up_p[3], up_p[0], color, line_size);

	//    Vertical Lines
	Vector<Vector2> down_p = *down_points;
	draw_line(up_p[0], down_p[0], color, line_size);
	draw_line(up_p[1], down_p[1], color, line_size);
	draw_line(up_p[2], down_p[2], color, line_size);
	draw_line(up_p[3], down_p[3], color, line_size);

	//    Lower Lines
	draw_line(down_p[0], down_p[1], color, line_size);
	draw_line(down_p[1], down_p[2], color, line_size);
	draw_line(down_p[2], down_p[3], color, line_size);
	draw_line(down_p[3], down_p[0], color, line_size);
}

void OutlineDrawer::setPoints(Vector<Vector2> *up, Vector<Vector2> *down) {
	up_points = up;
	down_points = down;
}

void OutlineDrawer::setColor(const Color &c) {
	color = c;
}

const Color &OutlineDrawer::get_color() const {
	return color;
}

void OutlineDrawer::setLineSize(real_t size) {
	line_size = size;
}

real_t OutlineDrawer::get_line_size() const {
	return line_size;
}

void OutlineDrawer::_bind_methods() {
}
