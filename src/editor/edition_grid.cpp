#ifdef TOOLS_ENABLED

#include "edition_grid.h"

using namespace editor;

EditionGrid::EditionGrid(int p_position, EditorAxes p_axes) : position{p_position}, axes{p_axes} {

}

int EditionGrid::get_position() const {
    return position;
}

EditorAxes EditionGrid::get_axes() const {
    return axes;
}

#endif
