#ifdef TOOLS_ENABLED

#ifndef ISOMETRIC_MAPS_FIX_SET_DIALOG_H
#define ISOMETRIC_MAPS_FIX_SET_DIALOG_H

#include <scene/gui/line_edit.h>
#include <scene/gui/button.h>
#include <scene/gui/item_list.h>
#include <scene/gui/dialogs.h>

namespace editor {
    namespace inspector {
        class FixSetDialog : public WindowDialog {
            GDCLASS(FixSetDialog, WindowDialog)

        public:
            FixSetDialog();
            ~FixSetDialog() override = default;

        private:
            LineEdit* add_group_path_line_edit;
            Button* add_group_path_button;

            ItemList* added_group_paths_item_list;
            ItemList* removed_elements_item_list;
            ItemList* new_elements_item_list;
            ItemList* fix_recap_item_list;

        public:
            static void _bind_methods();
        };
    }
}


#endif //ISOMETRIC_MAPS_FIX_SET_DIALOG_H

#endif
