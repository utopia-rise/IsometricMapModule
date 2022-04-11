#ifdef TOOLS_ENABLED

#ifndef ISOMETRIC_MAPS_EDITION_GRID_DRAWER_H
#define ISOMETRIC_MAPS_EDITION_GRID_DRAWER_H

#include <core/rid.h>
#include "editor_plane.h"
#include "../node/isometric_map.h"


namespace editor {
    class EditionGridDrawer {

    public:
        static void draw_grid(const EditorPlane &editor_plane, const node::IsometricMap& map) ;
        static void clear_grid(const EditorPlane &editor_plane) ;
    };
}


#endif //ISOMETRIC_MAPS_EDITION_GRID_DRAWER_H

#endif