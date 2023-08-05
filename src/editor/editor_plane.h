#ifndef ISOMETRIC_MAPS_EDITION_GRID_H
#define ISOMETRIC_MAPS_EDITION_GRID_H

#ifdef TOOLS_ENABLED

#include "core/templates/rid.h"

#include <core/math/vector2.h>
#include <core/math/vector3.h>

namespace editor {

    class EditorPlane {
    public:
        enum PlaneType {
            EDITOR_DRAWER,
            X_MIN_VIEW_LIMITER,
            X_MAX_VIEW_LIMITER,
            Y_MIN_VIEW_LIMITER,
            Y_MAX_VIEW_LIMITER,
            Z_MIN_VIEW_LIMITER,
            Z_MAX_VIEW_LIMITER,
            SIZE
        };

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
}// namespace editor

#endif
#endif// ISOMETRIC_MAPS_EDITION_GRID_H
