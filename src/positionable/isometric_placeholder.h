#ifndef ISOMETRIC_MAPS_ISOMETRIC_PLACEHOLDER_H
#define ISOMETRIC_MAPS_ISOMETRIC_PLACEHOLDER_H


#include "static_isometric_element.h"
#include "placeholder_type.h"

namespace positionable {

    class IsometricPlaceholder : public StaticIsometricElement {
        GDCLASS(IsometricPlaceholder, StaticIsometricElement)

    private:
        Ref<PlaceholderType> placeholderType;

        float tempAlpha = 0.15;

        PoolColorArray left_color;
        PoolColorArray right_color;
        PoolColorArray up_color;
        PoolColorArray down_color;
        PoolColorArray side_slope_color;
        PoolColorArray forward_slope_color;
        PoolColorArray backward_slope_color;
        PoolColorArray stair_color;

        Vector3 map_size;

        void draw_points();
        void update_colors();

        void set_map_size(const Vector3 &size);
    public:
        IsometricPlaceholder() = default;
        ~IsometricPlaceholder() override = default;

        void _init();
        void _draw();

        void on_grid_updated(int stair) override;
        void on_resize() override;

        Ref<PlaceholderType> get_placeholder_type() const;
        void set_placeholder_type(Ref<PlaceholderType> pType);

    protected:
        static void _bind_methods();
    };
}


#endif //ISOMETRIC_MAPS_ISOMETRIC_PLACEHOLDER_H
