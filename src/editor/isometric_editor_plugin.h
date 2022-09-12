#ifdef TOOLS_ENABLED

#ifndef ISOMETRIC_MAPS_ISOMETRIC_EDITOR_PLUGIN_H
#define ISOMETRIC_MAPS_ISOMETRIC_EDITOR_PLUGIN_H

#include <modules/isometric_maps/src/node/isometric_map.h>
#include <editor/editor_node.h>
#include <editor/editor_plugin.h>
#include <modules/isometric_maps/src/editor/commands/emitters/painting_command_emitter.h>
#include <modules/isometric_maps/src/editor/commands/emitters/select_command_emitter.h>
#include <modules/isometric_maps/src/editor/commands/emitters/delete_command_emitter.h>
#include <modules/isometric_maps/src/editor/commands/emitters/drag_and_drop_command_emitter.h>
#include <modules/isometric_maps/src/editor/commands/emitters/select_all_command_emitter.h>
#include <modules/isometric_maps/src/editor/commands/emitters/rotate_editor_plane_command_emitter.h>
#include <modules/isometric_maps/src/editor/commands/emitters/move_editor_grid_command_emitter.h>
#include "editor_plane.h"
#include "edition_grid_drawer.h"
#include "modules/isometric_maps/src/editor/inspector/positionable_selection_pane.h"
#include "modules/isometric_maps/src/editor/commands/emitters/move_editor_view_limiter_command_emitter.h"

namespace editor {

    class IsometricEditorPlugin : public EditorPlugin {
        GDCLASS(IsometricEditorPlugin, EditorPlugin)

    public:
        enum Mode {
            NONE,
            SELECT,
            PAINT,
            DRAG_AND_DROP
        };

        editor::inspector::PositionableSelectionPane* get_selection_pane() const;

        IsometricEditorPlugin();
        ~IsometricEditorPlugin() override;

        static IsometricEditorPlugin* get_instance();

        IsometricEditorPlugin(const IsometricEditorPlugin&) = delete;

        void set_debug_mode(bool b);
        void set_should_clear_buffer_on_next_frame(bool should);

        void refresh_positionable_selection_pane();
        void refresh() const;

        node::IsometricMap* get_selected_map() const;
        EditorPlane& get_editor_plane_for_selected_map(EditorPlane::PlaneType p_plane_type);

    protected:
        void _notification(int p_notification);

        bool forward_canvas_gui_input(const Ref<InputEvent>& p_event) override;

        void edit(Object* p_object) override;

        void drop();

        bool handles(Object* p_object) const override;

        void clear() override;

        void make_visible(bool b) override;

    private:
        struct MapHandlingData {
            EditorPlane editor_planes[EditorPlane::PlaneType::SIZE];

            MapHandlingData();
            explicit MapHandlingData(const Vector3& p_map_size);
        };

        editor::inspector::PositionableSelectionPane* positionable_selection_pane;
        ToolButton* positionable_pane_button;
        UndoRedo *undo_redo;
        HBoxContainer *toolbar;
        OptionButton* edition_mode_button;
        Button *debug_button;

        HashMap<uint64_t, MapHandlingData> handling_data_map;
        node::IsometricMap* selected_map;

        bool show_debug;
        Mode current_mode;

        bool should_clear_buffer_on_next_frame;

        commands::emitters::PaintingCommandEmitter painting_command_emitter;
        commands::emitters::SelectCommandEmitter select_command_emitter;
        commands::emitters::SelectAllCommandEmitter select_all_command_emitter;
        commands::emitters::DeleteCommandEmitter delete_command_emitter;
        commands::emitters::DragAndDropCommandEmitter drag_and_drop_command_emitter;
        commands::emitters::MoveEditorGridCommandEmitter move_editor_drawer_command_emitter;
        commands::emitters::RotateEditorPlaneCommandEmitter rotate_editor_plane_command_emitter;
        commands::emitters::MoveEditorViewLimiterCommandEmitter x_min_plane_view_limiter_command_emitter;
        commands::emitters::MoveEditorViewLimiterCommandEmitter x_max_plane_view_limiter_command_emitter;
        commands::emitters::MoveEditorViewLimiterCommandEmitter y_min_plane_view_limiter_command_emitter;
        commands::emitters::MoveEditorViewLimiterCommandEmitter y_max_plane_view_limiter_command_emitter;
        commands::emitters::MoveEditorViewLimiterCommandEmitter z_min_plane_view_limiter_command_emitter;
        commands::emitters::MoveEditorViewLimiterCommandEmitter z_max_plane_view_limiter_command_emitter;

        void _on_frame_post_draw();

        void _on_edition_mode_changed(int selected_index);

        void _draw_grids_and_planes() const;

    public:
        static void _bind_methods();
    };

}

#endif

#endif
