#ifndef ISOMETRIC_MAPS_LAYERS_EDITOR_H
#define ISOMETRIC_MAPS_LAYERS_EDITOR_H

#ifdef TOOLS_ENABLED
#include "scene/gui/box_container.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/button.h"
#include "scene/gui/check_box.h"
#include "scene/gui/grid_container.h"

namespace editor {
    namespace inspector {
        class LayersEditor : public VBoxContainer {
            GDCLASS(LayersEditor, VBoxContainer)

        public:
            void refresh();
            uint32_t get_selected_layer_id() const;

            LayersEditor();

        private:
            LineEdit* layer_line_edit;
            GridContainer* layer_controls_container;
            Ref<ButtonGroup> current_layer_button_group;

            void _add_layer();

        protected:
            static void _bind_methods();
        };

        class LayerVisibleCheckBox : public CheckBox {
            GDCLASS(LayerVisibleCheckBox, CheckBox)

        public:
            void set_layer_id(uint32_t p_layer_id);

            LayerVisibleCheckBox();

        private:
            uint32_t layer_id;

            void _set_layer_visible();

        protected:
            void _notification(int notif);
            static void _bind_methods();
        };

        class LayerRemoveButton : public Button {
            GDCLASS(LayerRemoveButton, Button)

        public:
            void set_layer_informations(uint32_t p_layer_id, const String& p_layer_name);

            LayerRemoveButton();

        private:
            String layer_name;
            uint32_t layer_id;

            void _remove_layer();

        protected:
            static void _bind_methods();
        };

        class CurrentLayerCheckBox : public CheckBox {
            GDCLASS(CurrentLayerCheckBox, CheckBox)

        public:
            uint32_t get_layer_id() const;
            void set_layer_id(uint32_t p_layer_id);

            CurrentLayerCheckBox();

        private:
            uint32_t layer_id;

        protected:
            static void _bind_methods();
        };
    }
}

#endif
#endif //ISOMETRIC_MAPS_LAYERS_EDITOR_H
