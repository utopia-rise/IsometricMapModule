#ifdef TOOLS_ENABLED

#ifndef ISOMETRIC_MAPS_ISOMETRIC_EDITOR_PLUGIN_H
#define ISOMETRIC_MAPS_ISOMETRIC_EDITOR_PLUGIN_H

#include <modules/isometric_maps/src/node/isometric_map.h>
#include <editor/editor_node.h>
#include <editor/editor_plugin.h>
#include "editor_plane.h"
#include "edition_grid_drawer.h"
#include "modules/isometric_maps/src/editor/inspector/positionable_selection_pane.h"

namespace editor {

    class IsometricEditorPlugin : public EditorPlugin {
        GDCLASS(IsometricEditorPlugin, EditorPlugin)

        enum Mode {
            NONE,
            SELECT,
            PAIN
        };

        struct MapHandlingData {
            EditorPlane edition_grid_plane;

            MapHandlingData();
            explicit MapHandlingData(EditorPlane p_editor_plane);
        };

        editor::inspector::PositionableSelectionPane* positionable_selection_pane;
        ToolButton* positionable_pane_button;
        UndoRedo *undo_redo;
        HBoxContainer *toolbar;
        Button *debug_button;

        HashMap<uint64_t, MapHandlingData> handling_data_map;
        node::IsometricMap* selected_map;

        bool show_debug;
        Mode current_mode;

        EditionGridDrawer edition_grid_drawer;

        bool should_clear_buffer_on_next_frame;

        void _on_frame_post_draw();

    public:
        IsometricEditorPlugin();
        ~IsometricEditorPlugin();

        static IsometricEditorPlugin* get_instance();

        IsometricEditorPlugin(const IsometricEditorPlugin&) = delete;

        void set_debug_mode(bool b);
        void set_should_clear_buffer_on_next_frame(bool should);

        void refresh_positionable_selection_pane();

    protected:
        void _notification(int p_notification);

        bool forward_canvas_gui_input(const Ref<InputEvent>& p_event) override;

        void edit(Object* p_object) override;

        void drop();

        bool handles(Object* p_object) const override;

        void clear() override;

        void make_visible(bool b) override;

        void refresh() const;

    public:
        static void _bind_methods();
    };

}

#endif

#endif
