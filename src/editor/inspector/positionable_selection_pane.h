#ifdef TOOLS_ENABLED

#ifndef ISOMETRIC_MAPS_POSITIONABLE_SELECTION_PANE_H
#define ISOMETRIC_MAPS_POSITIONABLE_SELECTION_PANE_H

#include <scene/gui/split_container.h>
#include <modules/isometric_maps/src/resource/positionable_set.h>
#include <scene/gui/option_button.h>
#include <scene/gui/panel.h>
#include <scene/gui/label.h>
#include <scene/gui/item_list.h>

namespace editor {
    namespace inspector {
        class PositionableSelectionPane : public VSplitContainer {
            GDCLASS(PositionableSelectionPane, VSplitContainer)
        public:
            void set_positionable_set(const Ref<resource::PositionableSet>& set);
            int get_selected_positionable_index() const;

            PositionableSelectionPane();
            ~PositionableSelectionPane() override = default;

        protected:
            void _notification(int notif);

        private:
            OptionButton* path_selector;
            ItemList* item_list;

            Ref<resource::PositionableSet> positionable_set;

            void _ready();
            void _refresh_path_selector();
            void _select_item_from_path_selector(int index);

            static Viewport* _get_icon_for_scene(Ref<PackedScene> scene);

        public:
            static void _bind_methods();
        };

    }
}

#endif //ISOMETRIC_MAPS_POSITIONABLE_SELECTION_PANE_H

#endif
