#ifdef TOOLS_ENABLED

#include "edition_grid_drawer.h"
#include "editor_plane.h"
#include "../isometric_server.h"

void editor::EditionGridDrawer::draw_grid(const EditorPlane& editor_plane, const node::IsometricMap& map) const {
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

    //TODO: Put that in the Transform2D above
    Vector2 offset{0, static_cast<float>(-diamond_height) * 0.5f};
    Vector2 height_offset{
            0,
            IsometricServer::get_instance()->get_isometric_space_z_length(space_rid) *
            static_cast<float>(editor_plane.get_position())
    };
    VisualServer::get_singleton()->canvas_item_set_transform(rid, Transform2D().translated( offset - height_offset));

    Vector3 map_size{map.get_size()};

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
    for (int i = 0; i < map.get_size().x + 1; i++) {
        auto index = static_cast<float>(i);

        Vector2 from{
                diamond_width * 0.5f * index, diamond_height * 0.5f * index
        };
        Vector2 to{
                diamond_width * 0.5f * (index - map_size.y), diamond_height * 0.5f * (map_size.y + index)
        };
        VisualServer::get_singleton()->canvas_item_add_line(rid, from, to, Color(0, 0, 0), 2.0);
    }
}

void editor::EditionGridDrawer::clear_grid(const editor::EditorPlane& editor_plane) const {
    RID rid{editor_plane.get_rid()};
    VisualServer::get_singleton()->canvas_item_clear(rid);
}

#endif
