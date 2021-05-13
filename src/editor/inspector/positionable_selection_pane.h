#ifdef TOOLS_ENABLED

#ifndef ISOMETRIC_MAPS_POSITIONABLE_SELECTION_PANE_H
#define ISOMETRIC_MAPS_POSITIONABLE_SELECTION_PANE_H

#include <scene/gui/split_container.h>
#include <modules/isometric_maps/src/resource/positionable_set.h>
#include <scene/gui/option_button.h>
#include <scene/gui/panel.h>
#include <scene/gui/label.h>
#include "positionable_set_inspector.h"

namespace editor {
    namespace inspector {
        class PositionableSelectionPane : public VSplitContainer {
            GDCLASS(PositionableSelectionPane, VSplitContainer)
        public:
            void set_positionable_set(const Ref<resource::PositionableSet>& set);

            PositionableSelectionPane();
            ~PositionableSelectionPane() override = default;

        protected:
            void _notification(int notif);

        private:
            OptionButton* path_selector;
            HSplitContainer* selectors_container;
            PositionableSetInspector* map_inspector;
            PositionableSetInspector* positionable_inspector;

            Ref<resource::PositionableSet> positionable_set;

            void _ready();
            void _refresh_path_selector();
            void _select_item_from_path_selector(int index);

            static void _full_inspector_update(PositionableSetInspector* inspector, const Vector<Ref<PackedScene>>& scenes);

        public:
            static void _bind_methods();
        };

    }
}

#endif //ISOMETRIC_MAPS_POSITIONABLE_SELECTION_PANE_H

#endif
