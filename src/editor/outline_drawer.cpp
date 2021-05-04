#include "outline_drawer.h"

using namespace editor;

OutlineDrawer::OutlineDrawer() :
        Node2D(), up_points(nullptr), down_points(nullptr), line_size(1) {
    set_z_index(4096);
}

void OutlineDrawer::_draw() {

    Vector<Vector2> up_p = *up_points;
    Vector<Vector2> down_p = *down_points;

    Color color1 = Color(0.8, 0.3, 0.3);
    Color color2 = Color(0.7, 0.3, 0.3);
    Color color3 = Color(0.6, 0.3, 0.3);

    auto upPoints = up_p;
    auto leftPoints = Vector<Vector2>();
    auto rightPoints = Vector<Vector2>();

    rightPoints.push_back(up_p[1]);
    rightPoints.push_back(down_p[1]);
    rightPoints.push_back(down_p[2]);
    rightPoints.push_back(up_p[2]);

    leftPoints.push_back(up_p[2]);
    leftPoints.push_back(down_p[2]);
    leftPoints.push_back(down_p[3]);
    leftPoints.push_back(up_p[3]);

    auto upColor = Vector<Color>();
    auto leftColor = Vector<Color>();
    auto rightColor = Vector<Color>();

    upColor.push_back(color1);
    upColor.push_back(color1);
    upColor.push_back(color1);
    upColor.push_back(color1);

    leftColor.push_back(color2);
    leftColor.push_back(color2);
    leftColor.push_back(color2);
    leftColor.push_back(color2);

    rightColor.push_back(color3);
    rightColor.push_back(color3);
    rightColor.push_back(color3);
    rightColor.push_back(color3);

    draw_polygon(upPoints, upColor);
    draw_polygon(leftPoints, leftColor);
    draw_polygon(rightPoints, rightColor);

    //    Upper Lines
    draw_line(up_p[0], up_p[1], color, line_size);
    draw_line(up_p[1], up_p[2], color, line_size);
    draw_line(up_p[2], up_p[3], color, line_size);
    draw_line(up_p[3], up_p[0], color, line_size);

    //    Vertical Lines

    //draw_line(up_p[0], down_p[0], color, line_size);
    draw_line(up_p[1], down_p[1], color, line_size);
    draw_line(up_p[2], down_p[2], color, line_size);
    draw_line(up_p[3], down_p[3], color, line_size);

    //    Lower Lines
    //draw_line(down_p[0], down_p[1], color, line_size);
    draw_line(down_p[1], down_p[2], color, line_size);
    draw_line(down_p[2], down_p[3], color, line_size);
    //draw_line(down_p[3], down_p[0], color, line_size);

}

void OutlineDrawer::setPoints(Vector<Vector2>* up, Vector<Vector2>* down) {
    up_points = up;
    down_points = down;
    update();
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

void OutlineDrawer::_notification(int notif) {
    switch (notif) {
        case NOTIFICATION_DRAW:
            _draw();
            break;
        default:
            break;
    }
}

void OutlineDrawer::_bind_methods() {
    ClassDB::bind_method(D_METHOD("_draw"), &OutlineDrawer::_draw);
}
