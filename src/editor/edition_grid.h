#include "editor_axes.h"

#ifdef TOOLS_ENABLED

#ifndef ISOMETRIC_MAPS_EDITIONGRID_H
#define ISOMETRIC_MAPS_EDITIONGRID_H

namespace editor {

    class EditionGrid {

    public:
        int get_position() const;
        EditorAxes get_axes() const;

        EditionGrid(int p_position, EditorAxes p_axes);
        ~EditionGrid() = default;

    private:
        int position;
        EditorAxes axes;
    };
}


#endif //ISOMETRIC_MAPS_EDITIONGRID_H
#endif
