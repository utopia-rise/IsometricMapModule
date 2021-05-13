#ifdef TOOLS_ENABLED

#ifndef GODOT_INSPECTOR_BUTTON_H
#define GODOT_INSPECTOR_BUTTON_H

#include <scene/gui/button.h>

namespace editor {
    namespace inspector {
        class InspectorButton : public Button {
            GDCLASS(InspectorButton, Button)

        public:
            InspectorButton();
            ~InspectorButton() override = default;

        public:
            static void _bind_methods();

        };
    }
}


#endif //GODOT_INSPECTOR_BUTTON_H

#endif