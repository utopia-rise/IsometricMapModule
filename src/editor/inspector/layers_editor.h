#ifndef ISOMETRIC_MAPS_LAYERS_EDITOR_H
#define ISOMETRIC_MAPS_LAYERS_EDITOR_H

#ifdef TOOLS_ENABLED
#include "node/isometric_map.h"
#include "scene/gui/color_picker.h"
#include "scene/gui/dialogs.h"

#include <scene/gui/box_container.h>
#include <scene/gui/button.h>
#include <scene/gui/check_box.h>
#include <scene/gui/grid_container.h>
#include <scene/gui/line_edit.h>

namespace editor {
    namespace inspector {
        class LayersEditor : public VBoxContainer {
            GDCLASS(LayersEditor, VBoxContainer)

        public:
            void refresh();

            LayersEditor();

        private:
            LineEdit* layer_line_edit {memnew(LineEdit)};
            GridContainer* layer_controls_container {memnew(GridContainer)};
            ConfirmationDialog* remove_layer_popup {memnew(ConfirmationDialog)};
            Label* remove_layer_popup_label {memnew(Label)};
            Ref<ButtonGroup> current_layer_button_group;

            void _add_layer();
            void _on_remove_layer_button(uint32_t p_layer_id, const String& p_layer_name);
            void _on_layer_name_changed(const String& p_layer_name, uint32_t p_layer_id);
            void _on_layer_select_content(uint32_t p_layer_id);
            void _set_layer_visible(uint32_t p_layer_id, CheckBox* p_check_box);
            void _set_current_layer(uint32_t p_layer_id);
            void _layer_color_changed(const Color& p_color, uint32_t p_layer_id);
            void _remove_layer(uint32_t p_layer_id, const String& p_layer_name);
        };
    }// namespace inspector
}// namespace editor

#endif
#endif// ISOMETRIC_MAPS_LAYERS_EDITOR_H
