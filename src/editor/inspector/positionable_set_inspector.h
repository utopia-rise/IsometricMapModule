#ifdef TOOLS_ENABLED

#ifndef ISOMETRIC_MAPS_POSITIONABLE_SET_INSPECTOR_H
#define ISOMETRIC_MAPS_POSITIONABLE_SET_INSPECTOR_H

#include <scene/gui/panel.h>
#include <scene/gui/label.h>
#include <scene/gui/scroll_container.h>
#include <scene/gui/box_container.h>
#include <scene/main/viewport.h>
#include "inspector_button.h"

namespace editor {
    namespace inspector {

        class PositionableSetInspector : public VBoxContainer {
        GDCLASS(PositionableSetInspector, VBoxContainer)

        public:
            void update_for_positionable(Ref<PackedScene> positionable_scene);

            void clear();

            explicit PositionableSetInspector(const String &title);

            PositionableSetInspector();

            ~PositionableSetInspector() override = default;

        private:
            Label* title_label;
            ScrollContainer *scroll_container;
            VBoxContainer* viewer;

            HashMap<StringName, inspector::InspectorButton*> buttons;

            void _init();
            static Viewport * _get_icon_for_scene(Ref<PackedScene> scene);

        public:
            static void _bind_methods();

        };

    }
}


#endif //ISOMETRIC_MAPS_POSITIONABLE_SET_INSPECTOR_H

#endif
