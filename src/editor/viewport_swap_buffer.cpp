#ifdef TOOLS_ENABLED

#include "viewport_swap_buffer.h"
#include <editor/editor_node.h>

using namespace editor;

void ViewportSwapBuffer::push_back(Viewport* element) {
    Vector<Viewport*>& acquisition_buffer{is_first_in_use ? second_data : first_data};
    print_line(vformat("Adding viewport: %s", element));
    acquisition_buffer.push_back(element);
}

void ViewportSwapBuffer::clear() {
    Vector<Viewport*>& in_use_buffer{is_first_in_use ? first_data : second_data};
    for (int i = 0; i < in_use_buffer.size(); ++i) {
        Viewport* viewport{in_use_buffer[i]};
        print_line(vformat("Clearing viewport: %s", viewport));
        EditorNode::get_singleton()->remove_child(viewport);
        viewport->queue_delete();
        in_use_buffer.remove(i);
        --i;
    }
}

void ViewportSwapBuffer::swap() {
    if (!is_locked) {
        Vector<Viewport *> &acquisition_buffer{is_first_in_use ? second_data : first_data};
        if (!acquisition_buffer.empty()) {
            clear();
            is_first_in_use = !is_first_in_use;
            Vector<Viewport *> &in_use_buffer{is_first_in_use ? first_data : second_data};
            for (int i = 0; i < in_use_buffer.size(); ++i) {
                print_line(vformat("Adding viewport as child: %s", in_use_buffer[i]));
                EditorNode::get_singleton()->add_child(in_use_buffer[i]);
            }
        }
    }
}

void ViewportSwapBuffer::lock() {
    is_locked = true;
}

void ViewportSwapBuffer::unlock() {
    is_locked = false;
}

ViewportSwapBuffer::ViewportSwapBuffer() : first_data(), second_data(), is_first_in_use(), is_locked() {

}

#endif
