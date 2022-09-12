#ifdef TOOLS_ENABLED

#include "edition_grid_drawer.h"
#include "editor_plane.h"
#include "../isometric_server.h"

void editor::EditionGridDrawer::draw_grid(const EditorPlane& editor_plane, const node::IsometricMap& map) {
    RID rid{editor_plane.get_rid()};
    VisualServer::get_singleton()->canvas_item_clear(rid);
    VisualServer::get_singleton()->canvas_item_set_parent(rid, map.get_canvas_item());


    RID space_rid{map.get_space_RID()};

    float diamond_height{
        static_cast<float>(IsometricServer::get_instance()->get_isometric_space_diamond_height(space_rid))
    };
    float diamond_width{
        static_cast<float>(IsometricServer::get_instance()->get_isometric_space_diamond_width(space_rid))
    };
    float tile_z_length{IsometricServer::get_instance()->get_isometric_space_z_length(space_rid)};

    Vector3 map_size{map.get_size()};

    Vector2 global_offset{0, static_cast<float>(-diamond_height) * 0.5f};
    float editor_plane_position = static_cast<float>(editor_plane.get_position());

    switch (editor_plane.get_axis()) {
        case Vector3::AXIS_X:
            {
                Vector2 offset{
                        -diamond_width * 0.5f * editor_plane_position,
                        -diamond_height * 0.5f * editor_plane_position
                };
                VisualServer::get_singleton()->canvas_item_set_transform(rid, Transform2D().translated(global_offset - offset));
            }

            for (int i = 0; i < static_cast<int>(map_size.y) + 1; i++) {
                auto index = static_cast<float>(i);

                Vector2 from{
                        -diamond_width * 0.5f * index, diamond_height * 0.5f * index
                };
                Vector2 to{
                        -diamond_width * 0.5f * index, diamond_height * 0.5f * index - tile_z_length * map_size.z
                };
                VisualServer::get_singleton()->canvas_item_add_line(rid, from, to, Color(0, 0, 0), 2.0);
            }
            for (int i = 0; i < static_cast<int>(map_size.z) + 1; i++) {
                auto index = static_cast<float>(i);

                Vector2 from{
                        0, -tile_z_length * index
                };
                Vector2 to{
                        -diamond_width * 0.5f * map_size.y, diamond_height * 0.5f * map_size.y - tile_z_length * index
                };
                VisualServer::get_singleton()->canvas_item_add_line(rid, from, to, Color(0, 0, 0), 2.0);
            }
            break;
        case Vector3::AXIS_Y:
            {
                Vector2 offset{
                        diamond_width * 0.5f * editor_plane_position,
                        -diamond_height * 0.5f * editor_plane_position
                };
                VisualServer::get_singleton()->canvas_item_set_transform(rid, Transform2D().translated(global_offset - offset));
            }

            for (int i = 0; i < static_cast<int>(map_size.z) + 1; i++) {
                auto index = static_cast<float>(i);

                Vector2 from{
                        0, -tile_z_length * index
                };
                Vector2 to{
                        diamond_width * 0.5f * (map_size.x), diamond_height * 0.5f * map_size.x - tile_z_length * index
                };
                VisualServer::get_singleton()->canvas_item_add_line(rid, from, to, Color(0, 0, 0), 2.0);
            }
            for (int i = 0; i < static_cast<int>(map_size.x) + 1; i++) {
                auto index = static_cast<float>(i);

                Vector2 from{
                        diamond_width * 0.5f * index, diamond_height * 0.5f * index
                };
                Vector2 to{
                        diamond_width * 0.5f * index, diamond_height * 0.5f * index - tile_z_length * map_size.z
                };
                VisualServer::get_singleton()->canvas_item_add_line(rid, from, to, Color(0, 0, 0), 2.0);
            }
            break;
        case Vector3::AXIS_Z:
            {
                Vector2 offset{
                        0,
                        IsometricServer::get_instance()->get_isometric_space_z_length(space_rid) *
                        editor_plane_position
                };
                VisualServer::get_singleton()->canvas_item_set_transform(rid, Transform2D().translated(global_offset - offset));
            }

            for (int i = 0; i < static_cast<int>(map_size.y) + 1; i++) {
                auto index = static_cast<float>(i);

                Vector2 from{
                        -diamond_width * 0.5f * index, diamond_height * 0.5f * index
                };
                Vector2 to{
                        diamond_width * 0.5f * (map_size.x - index), diamond_height * 0.5f * (index + map_size.x)
                };
                VisualServer::get_singleton()->canvas_item_add_line(rid, from, to, Color(0, 0, 0), 2.0);
            }
            for (int i = 0; i < static_cast<int>(map_size.x) + 1; i++) {
                auto index = static_cast<float>(i);

                Vector2 from{
                        diamond_width * 0.5f * index, diamond_height * 0.5f * index
                };
                Vector2 to{
                        diamond_width * 0.5f * (index - map_size.y), diamond_height * 0.5f * (map_size.y + index)
                };
                VisualServer::get_singleton()->canvas_item_add_line(rid, from, to, Color(0, 0, 0), 2.0);
            }
            break;
    }
}

void editor::EditionGridDrawer::draw_plane(const editor::EditorPlane& p_editor_plane, const node::IsometricMap& map) {
    RID rid{p_editor_plane.get_rid()};

    VisualServer::get_singleton()->canvas_item_clear(rid);
    VisualServer::get_singleton()->canvas_item_set_parent(rid, map.get_canvas_item());

    RID space_rid{map.get_space_RID()};
    float diamond_width{
            static_cast<float>(IsometricServer::get_instance()->get_isometric_space_diamond_width(space_rid))
    };
    float diamond_height{
            static_cast<float>(IsometricServer::get_instance()->get_isometric_space_diamond_height(space_rid))
    };
    Vector3 map_size{map.get_size()};
    int editor_plane_position{p_editor_plane.get_position()};
    Vector2 global_offset{0, static_cast<float>(-diamond_height) * 0.5f};
    float tile_z_length{IsometricServer::get_instance()->get_isometric_space_z_length(space_rid)};

    Vector<Point2> polygon_points;

    switch (p_editor_plane.get_axis()) {
        case Vector3::AXIS_X:
            {
                Vector2 offset{
                        -diamond_width * 0.5f * editor_plane_position,
                        -diamond_height * 0.5f * editor_plane_position
                };
                VisualServer::get_singleton()->canvas_item_set_transform(rid, Transform2D().translated(global_offset - offset));
            }
            polygon_points.push_back(
                    {0, 0}
            );
            polygon_points.push_back(
                    {0, -tile_z_length * map_size.z}
            );
            polygon_points.push_back(
                    {-diamond_width * 0.5f * map_size.y, diamond_height * 0.5f * map_size.y - tile_z_length * map_size.z}
            );
            polygon_points.push_back(
                    {-diamond_width * 0.5f * map_size.y, diamond_height * 0.5f * map_size.y}
            );
            polygon_points.push_back(
                    {0, 0}
            );
            break;
        case Vector3::AXIS_Y:
            {
                Vector2 offset{
                        diamond_width * 0.5f * editor_plane_position,
                        -diamond_height * 0.5f * editor_plane_position
                };
                VisualServer::get_singleton()->canvas_item_set_transform(rid, Transform2D().translated(global_offset - offset));
            }

            polygon_points.push_back(
                    {0, 0}
            );
            polygon_points.push_back(
                    {diamond_width * 0.5f * (map_size.x), diamond_height * 0.5f * map_size.x}
            );
            polygon_points.push_back(
                    {diamond_width * 0.5f * (map_size.x), diamond_height * 0.5f * map_size.x - tile_z_length * map_size.z}
            );
            polygon_points.push_back(
                    {0, -tile_z_length * map_size.z}
            );
            polygon_points.push_back(
                    {0, 0}
            );
            break;
        case Vector3::AXIS_Z:
            {
                Vector2 offset{
                        0,
                        IsometricServer::get_instance()->get_isometric_space_z_length(space_rid) *
                        editor_plane_position
                };
                VisualServer::get_singleton()->canvas_item_set_transform(rid, Transform2D().translated(global_offset - offset));
            }
            polygon_points.push_back(
                    {0, 0}
            );
            polygon_points.push_back(
                    {-diamond_width * 0.5f * map_size.y, diamond_height * 0.5f * map_size.y}
            );
            polygon_points.push_back(
                    {diamond_width * 0.5f * (map_size.x - map_size.y), diamond_height * 0.5f * (map_size.y + map_size.x)}
            );
            polygon_points.push_back(
                    {diamond_width * 0.5f * map_size.x, diamond_height * 0.5f * map_size.x}
            );
            polygon_points.push_back(
                    {0, 0}
            );
            break;
    }
    Vector<Color> colors;
    colors.push_back(Color(0, 0, 0, 0.2));
    VisualServer::get_singleton()->canvas_item_add_polygon(p_editor_plane.get_rid(), polygon_points, colors);
}

void editor::EditionGridDrawer::clear_for_editor_plane(const editor::EditorPlane& editor_plane) {
    RID rid{editor_plane.get_rid()};
    VisualServer::get_singleton()->canvas_item_clear(rid);
}

#endif
