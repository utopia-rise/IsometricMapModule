#ifndef ISOMETRIC_MAPS_ISOMETRIC_PLACEHOLDER_H
#define ISOMETRIC_MAPS_ISOMETRIC_PLACEHOLDER_H


#include "static_isometric_element.h"
#include "placeholder_type.h"

namespace positionable {

    class IsometricPlaceholder : public StaticIsometricElement {
        GDCLASS(IsometricPlaceholder, StaticIsometricElement)

    private:
        Ref<PlaceholderType> placeholder_type;

        float tempAlpha = 0.15;

        Vector<Color> left_color;
        Vector<Color> right_color;
        Vector<Color> up_color;
        Vector<Color> down_color;
        Vector<Color> side_slope_color;
        Vector<Color> forward_slope_color;
        Vector<Color> backward_slope_color;
        Vector<Color> stair_color;

        Vector3 map_size;

        void draw_points();
        void update_colors();

        void set_map_size(const Vector3 &size);
    public:
        IsometricPlaceholder();
        ~IsometricPlaceholder() override = default;

        void _draw();

        void on_grid_updated(int stair) override;
        void on_resize() override;

        Ref<PlaceholderType> get_placeholder_type() const;
        void set_placeholder_type(Ref<PlaceholderType> p_type);

    protected:
        static void _bind_methods();
    };
}


#endif //ISOMETRIC_MAPS_ISOMETRIC_PLACEHOLDER_H
