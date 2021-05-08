#ifdef TOOLS_ENABLED

#include "edition_grid_drawer.h"
#include "editor_plane.h"

void editor::EditionGridDrawer::draw_grid(const EditorPlane& editor_plane, const node::IsometricMap& map) {
    RID rid{editor_plane.get_rid()};
    VisualServer::get_singleton()->canvas_item_clear(rid);
    VisualServer::get_singleton()->canvas_item_set_parent(rid, map.get_canvas_item());
    VisualServer::get_singleton()->canvas_item_set_transform(rid, Transform2D());

    int tile_depth{IsoServer.tile_height};
    int tile_width{IsoServer.tile_width};
    Vector2 offset{Vector2(0, -tile_depth * 0.5)};
    Vector2 height_offset{Vector2(0, IsoServer.get_ez() * editor_plane.get_size())};
    for (int i = 0; i < map_depth + 1; i++) {
        Vector2 from{
                Vector2(-tile_width * 0.5 * i, tile_depth * 0.5 * i) + offset - height_offset
        };
        Vector2 to{
                Vector2(tile_width * 0.5 * (map_width - i), tile_depth * 0.5 * (i + map_width)) + offset - height_offset
        };
        VisualServer::get_singleton()->canvas_item_add_line(rid, from, to, Color(0, 0, 0), 2.0);
    }
    for (int i = 0; i < map_witdh + 1; i++) {
        Vector2 from{
                Vector2(tile_width * 0.5 * i, tile_depth * 0.5 * i) + offset - height_offset
        };
        Vector2 to{
                Vector2(tile_width * 0.5 * (i - map_depth), tile_depth * 0.5 * (map_depth + i)) + offset - height_offset
        };
        VisualServer::get_singleton()->canvas_item_add_line(rid, from, to, Color(0, 0, 0), 2.0);
    }
}

#endif
