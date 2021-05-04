#ifdef TOOLS_ENABLED

#ifndef ISOMETRIC_MAPS_EDITIONGRID_H
#define ISOMETRIC_MAPS_EDITIONGRID_H

#include "editor_axes.h"
#include <core/math/vector2.h>

namespace editor {

    class EditorPlane {

    public:
        int get_position() const;
        EditorAxes get_axes() const;
        const Vector2& get_size() const;

        EditorPlane(int p_position, EditorAxes p_axes, const Vector2& p_size);
        ~EditorPlane() = default;

    private:
        int position;
        EditorAxes axes;
        Vector2 size;
    };
}


#endif //ISOMETRIC_MAPS_EDITIONGRID_H
#endif
