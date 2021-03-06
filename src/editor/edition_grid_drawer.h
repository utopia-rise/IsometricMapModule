#ifdef TOOLS_ENABLED

#include <core/rid.h>
#include "editor_plane.h"
#include "../node/isometric_map.h"

#ifndef ISOMETRIC_MAPS_EDITION_GRID_DRAWER_H
#define ISOMETRIC_MAPS_EDITION_GRID_DRAWER_H


namespace editor {
    class EditionGridDrawer {

    public:
        void draw_grid(const EditorPlane &editor_plane, const node::IsometricMap& map) const;
        void clear_grid(const EditorPlane &editor_plane) const;
    };
}


#endif //ISOMETRIC_MAPS_EDITION_GRID_DRAWER_H

#endif