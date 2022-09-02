#ifdef TOOLS_ENABLED

#ifndef ISOMETRIC_MAPS_EDITIONGRID_H
#define ISOMETRIC_MAPS_EDITIONGRID_H

#include <core/math/vector2.h>
#include <core/rid.h>
#include <core/math/vector3.h>

namespace editor {

    class EditorPlane {

    public:
        int get_position() const;
        void set_position(int p_position);
        Vector3::Axis get_axis() const;
        void set_axis(Vector3::Axis p_axis);
        RID get_rid() const;
        const Vector2& get_size() const;

        EditorPlane(int p_position, Vector3::Axis p_axis, const Vector2& p_size);
        ~EditorPlane() = default;

    private:
        int position;
        Vector3::Axis axis;
        Vector2 size;
        RID self;
    };
}


#endif //ISOMETRIC_MAPS_EDITIONGRID_H
#endif
