#ifndef ISOMETRIC_MAPS_POSITIONABLE_SELECTION_PANE_H
#define ISOMETRIC_MAPS_POSITIONABLE_SELECTION_PANE_H

#ifdef TOOLS_ENABLED

    #include <modules/isometric_maps/src/resource/positionable_set.h>
    #include <scene/gui/item_list.h>
    #include <scene/gui/label.h>
    #include <scene/gui/option_button.h>
    #include <scene/gui/panel.h>
    #include <scene/gui/split_container.h>

namespace editor {
    namespace inspector {
        class PositionableSelectionPane : public VSplitContainer {
            GDCLASS(PositionableSelectionPane, VSplitContainer)

        public:
            void set_positionable_set(const Ref<resource::PositionableSet>& set);
            int get_selected_positionable_id() const;

            void refresh_path_selector();

            PositionableSelectionPane();
            ~PositionableSelectionPane() override;

        protected:
            void _notification(int notif);

        private:
            HSplitContainer* top_container;
            OptionButton* category_selector;
            Button* refresh_button;
            ItemList* item_list;

            Ref<resource::PositionableSet> positionable_set;

            void _ready();
            void _select_item_from_path_selector(int index);

            void _refresh_icons();

        public:
            static void _bind_methods();
        };

    }// namespace inspector
}// namespace editor

#endif
#endif// ISOMETRIC_MAPS_POSITIONABLE_SELECTION_PANE_H
