#ifdef TOOLS_ENABLED

#ifndef ISOMETRIC_MAPS_FIX_SET_DIALOG_H
#define ISOMETRIC_MAPS_FIX_SET_DIALOG_H

#include <scene/gui/line_edit.h>
#include <scene/gui/button.h>
#include <scene/gui/item_list.h>
#include <scene/gui/dialogs.h>
#include <modules/isometric_maps/src/resource/positionable_set.h>

namespace editor {
    namespace inspector {
        class FixSetDialog : public WindowDialog {
            GDCLASS(FixSetDialog, WindowDialog)

        public:
            void setup(Ref<resource::PositionableSet> positionable_set);
            void reset();

            FixSetDialog();
            ~FixSetDialog() override = default;

        protected:
            void _notification(int notif);

        private:
            class AssociationMetadata : public Reference {
            public:
                int get_id() const;
                void set_id(int p_id);
                const String& get_removed_path() const;
                void set_removed_path(const String& p_removed_path);
                const String& get_new_path() const;
                void set_new_path(const String& p_new_path);
                const String& get_path_group() const;
                void set_path_group(const String& p_path_group);

                AssociationMetadata();
                ~AssociationMetadata() override = default;

            private:
                int id;
                String removed_path;
                String new_path;
                String path_group;
            };

            LineEdit* add_group_path_line_edit;

            ItemList* added_group_paths_item_list;
            ItemList* removed_elements_item_list;
            ItemList* new_elements_item_list;
            ItemList* fix_recap_item_list;
            Button* validate_button;

            Ref<resource::PositionableSet> current_positionable_set;
            Vector<String> already_presents;

            void _on_add_group_path_button();
            void _on_remove_button();
            void _on_new_association_button();
            void _on_revert_association_button();
            void _on_validate_button();

            void _add_new_association(const Ref<AssociationMetadata>& association_metadata);
        public:
            static void _bind_methods();
        };
    }
}


#endif //ISOMETRIC_MAPS_FIX_SET_DIALOG_H

#endif
