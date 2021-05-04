#ifdef TOOLS_ENABLED

#include "edition_grid_drawer.h"
#include "editor_plane.h"
#include <servers/visual_server.h>

void editor::EditionGridDrawer::draw_grid(const EditorPlane& editor_plane) {
    drawing_rid = VisualServer::get_singleton()->canvas_item_create();
    //TODO: draw line using editor_plane and add rid.
}

#endif
