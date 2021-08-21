#ifndef ISOMETRIC_MAPS_OUTLINE_DRAWER_H
#define ISOMETRIC_MAPS_OUTLINE_DRAWER_H

#include <scene/2d/node_2d.h>

namespace editor {
    class OutlineDrawer : public Node2D {
    GDCLASS(OutlineDrawer, Node2D)

    private:
        Vector<Vector2>* up_points;
        Vector<Vector2>* down_points;
        Color color;
        real_t line_size;
        bool should_draw_polygons;

    public:
        OutlineDrawer();

        ~OutlineDrawer() override = default;

        void _draw();

        void set_points(Vector<Vector2>* up, Vector<Vector2>* down);

        void set_color(const Color &c);

        const Color &get_color() const;

        void set_line_size(real_t size);

        real_t get_line_size() const;

        void set_should_draw_polygons(bool should);

    protected:
        static void _bind_methods();

        void _notification(int notif);
    };
} // namespace editor

#endif //ISOMETRIC_MAPS_OUTLINE_DRAWER_H
