#ifdef TOOLS_ENABLED

#include <core/rid.h>
#include "editor_plane.h"

#ifndef ISOMETRIC_MAPS_EDITION_GRID_DRAWER_H
#define ISOMETRIC_MAPS_EDITION_GRID_DRAWER_H


namespace editor {
    class EditionGridDrawer {
        RID drawing_rid;

    public:
        void draw_grid(const EditorPlane &editor_plane);
    };
}


#endif //ISOMETRIC_MAPS_EDITION_GRID_DRAWER_H

#endif