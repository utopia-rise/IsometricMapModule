#ifndef ISOMETRIC_MAPS_ISOMETRIC_EDITOR_PLUGIN_H
#define ISOMETRIC_MAPS_ISOMETRIC_EDITOR_PLUGIN_H

#include <modules/isometric_maps/src/node/isometric_map.h>
#include "editor/editor_node.h"
#include "editor/editor_plugin.h"
#include "edition_grid.h"

namespace editor {

    class IsometricEditorPlugin : public EditorPlugin {
        GDCLASS(IsometricEditorPlugin, EditorPlugin)

        enum Mode {
            NONE,
            SELECT,
            PAIN
        };

        struct MapHandlingData {
            EditionGrid edition_grid;
        };

        IsometricEditorPlugin();

        UndoRedo *undo_redo;
        HBoxContainer *toolbar;
        Button *debug_button;

        HashMap<node::IsometricMap*, MapHandlingData> handling_data_map;
        node::IsometricMap* selected_map;

        bool show_debug;
        Mode current_mode;

    public:
        static IsometricEditorPlugin* get_instance();

        IsometricEditorPlugin(const IsometricEditorPlugin&) = delete;

        void set_debug_mode(bool b);

    protected:
        void _notification(int p_notification);

        bool forward_canvas_gui_input(const Ref<InputEvent>& p_event) override;

        void edit(Object* p_object) override;

        bool handles(Object* p_object) const override;

        void clear() override;

        void make_visible(bool b) override;

    public:
        static void _bind_methods();
    };

}

#endif
