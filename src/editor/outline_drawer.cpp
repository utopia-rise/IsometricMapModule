#ifdef TOOLS_ENABLED

    #include "outline_drawer.h"

    #include "isometric_server.h"
    #include "utils/isometric_maths.h"

using namespace editor;

void OutlineDrawer::draw_outline(node::IsometricPositionable* positionable) {
    const OutlineData& outline_data {positionable->get_outline_data()};
    RID outline_rid {outline_data.rid};
    VisualServer::get_singleton()->canvas_item_clear(outline_rid);
    VisualServer::get_singleton()->canvas_item_set_parent(outline_rid, positionable->get_canvas_item());
    VisualServer::get_singleton()->canvas_item_set_z_as_relative_to_parent(outline_rid, true);

    if (const data::IsometricParameters
        * space_configuration {
          IsometricServer::get_instance()->space_get_configuration(positionable->get_space_RID())}) {
        PoolVector2Array points {utils::get_bounding_box(*space_configuration, positionable->get_size())};

        int no_slope {0};
        int left_slope {0};
        int right_slope {0};
        int forward_slope {0};
        int backward_slope {0};

        switch (positionable->get_slope_type()) {
            case node::IsometricPositionable::SlopeType::NONE:
                no_slope = 1;
                break;
            case node::IsometricPositionable::SlopeType::LEFT:
                left_slope = 1;
                break;
            case node::IsometricPositionable::SlopeType::RIGHT:
                right_slope = 1;
                break;
            case node::IsometricPositionable::SlopeType::FORWARD:
                forward_slope = 1;
                break;
            case node::IsometricPositionable::SlopeType::BACKWARD:
                backward_slope = 1;
                break;
            case node::IsometricPositionable::SlopeType::SLOPE_TYPE_MAX:
                break;
        }

        Vector<Vector2> up_points;
        Vector<Vector2> down_points;

        up_points.resize(0);
        up_points.push_back((no_slope + left_slope + forward_slope) * points[4] + (right_slope + backward_slope) * points[0]);
        up_points.push_back((no_slope + right_slope + forward_slope) * points[5] + (left_slope + backward_slope) * points[1]);
        up_points.push_back((no_slope + right_slope + backward_slope) * points[6] + (left_slope + forward_slope) * points[2]);
        up_points.push_back((no_slope + left_slope + backward_slope) * points[7] + (right_slope + forward_slope) * points[3]);

        down_points.resize(0);
        down_points.push_back(points[0]);
        down_points.push_back(points[1]);
        down_points.push_back(points[2]);
        down_points.push_back(points[3]);

        Color color1;
        Color color2;
        Color color3;

        if (positionable->get_is_dynamic()) {
            color1 = {0.5, 0.5, 0.5, 1.};
            color2 = {0.2, 0.2, 0.2, 1.};
            color3 = {0.4, 0.4, 0.4, 1.};
        } else {
            color1 = {0.9, 0.9, 0.9, 1.};
            color2 = {0.6, 0.6, 0.6, 1.};
            color3 = {0.8, 0.8, 0.8, 1.};
        }

        auto leftPoints = Vector<Vector2>();
        auto rightPoints = Vector<Vector2>();

        rightPoints.push_back(up_points[1]);
        rightPoints.push_back(down_points[1]);
        rightPoints.push_back(down_points[2]);
        rightPoints.push_back(up_points[2]);

        leftPoints.push_back(up_points[2]);
        leftPoints.push_back(down_points[2]);
        leftPoints.push_back(down_points[3]);
        leftPoints.push_back(up_points[3]);

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

        if (outline_data.should_draw_polygons) {
            VisualServer::get_singleton()->canvas_item_add_polygon(outline_rid, up_points, upColor);
            VisualServer::get_singleton()->canvas_item_add_polygon(outline_rid, leftPoints, leftColor);
            VisualServer::get_singleton()->canvas_item_add_polygon(outline_rid, rightPoints, rightColor);
        }

        const Color& color {outline_data.color};
        real_t line_size {outline_data.line_size};

        //    Upper Lines
        VisualServer::get_singleton()->canvas_item_add_line(outline_rid, up_points[0], up_points[1], color, line_size);
        VisualServer::get_singleton()->canvas_item_add_line(outline_rid, up_points[1], up_points[2], color, line_size);
        VisualServer::get_singleton()->canvas_item_add_line(outline_rid, up_points[2], up_points[3], color, line_size);
        VisualServer::get_singleton()->canvas_item_add_line(outline_rid, up_points[3], up_points[0], color, line_size);

        //    Vertical Lines

        // draw_line(up_p[0], down_p[0], color, line_size);
        VisualServer::get_singleton()->canvas_item_add_line(outline_rid, up_points[1], down_points[1], color, line_size);
        VisualServer::get_singleton()->canvas_item_add_line(outline_rid, up_points[2], down_points[2], color, line_size);
        VisualServer::get_singleton()->canvas_item_add_line(outline_rid, up_points[3], down_points[3], color, line_size);

        //    Lower Lines
        // draw_line(down_p[0], down_p[1], color, line_size);
        VisualServer::get_singleton()->canvas_item_add_line(outline_rid, down_points[1], down_points[2], color, line_size);
        VisualServer::get_singleton()->canvas_item_add_line(outline_rid, down_points[2], down_points[3], color, line_size);
        // draw_line(down_p[3], down_p[0], color, line_size);

        VisualServer::get_singleton()->canvas_item_set_visible(outline_rid, outline_data.is_visible);
    }
}

void OutlineDrawer::set_outline_visible(node::IsometricPositionable* positionable, bool visible) {
    OutlineData& outline_data {positionable->get_outline_data()};
    outline_data.is_visible = visible;
    VisualServer::get_singleton()->canvas_item_set_visible(outline_data.rid, visible);
}

#endif