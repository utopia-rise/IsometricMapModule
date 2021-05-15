#ifdef TOOLS_ENABLED

#ifndef ISOMETRIC_MAPS_VIEWPORT_SWAP_BUFFER_H
#define ISOMETRIC_MAPS_VIEWPORT_SWAP_BUFFER_H

#include <core/vector.h>
#include <scene/main/viewport.h>


namespace editor {
    class ViewportSwapBuffer {
    private:
    public:
        void push_back(Viewport* element);
        void swap();
        void lock();
        void unlock();

        ViewportSwapBuffer();
        ~ViewportSwapBuffer() = default;

    private:
        void clear();

        Vector<Viewport*> first_data;
        Vector<Viewport*> second_data;
        bool is_first_in_use;

        bool is_locked;
    };
}


#endif //ISOMETRIC_MAPS_VIEWPORT_SWAP_BUFFER_H

#endif