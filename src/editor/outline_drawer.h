#ifndef ISOMETRIC_MAPS_OUTLINE_DRAWER_H
#define ISOMETRIC_MAPS_OUTLINE_DRAWER_H


#include <scene/2d/node_2d.h>

namespace editor {
    class OutlineDrawer : public Node2D {
        GDCLASS(OutlineDrawer, Node2D)

    private:
        PoolVector2Array* upPoints;
        PoolVector2Array* downPoints;
        Color color;
        real_t lineSize;

    public:
        OutlineDrawer();
        ~OutlineDrawer() = default;

        void _init();
        void _draw();
        void setPoints(PoolVector2Array* up, PoolVector2Array* down);
        void setColor(const Color& c);
        const Color& getColor() const;
        void setLineSize(real_t size);
        real_t getLineSize() const;

    protected:
        static void _bind_methods();
    };
}


#endif //ISOMETRIC_MAPS_OUTLINE_DRAWER_H
