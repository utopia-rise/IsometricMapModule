#ifndef ISOMETRIC_MAPS_ISOMETRIC_EDITOR_PLUGIN_H
#define ISOMETRIC_MAPS_ISOMETRIC_EDITOR_PLUGIN_H

#ifdef TOOLS_ENABLED

#include "edition_grid_drawer.h"
#include "editor/commands/emitters/delete_command_emitter.h"
#include "editor/commands/emitters/drag_and_drop_command_emitter.h"
#include "editor/commands/emitters/move_editor_grid_command_emitter.h"
#include "editor/commands/emitters/painting_command_emitter.h"
#include "editor/commands/emitters/rotate_editor_plane_command_emitter.h"
#include "editor/commands/emitters/select_all_command_emitter.h"
#include "editor/commands/emitters/select_command_emitter.h"
#include "editor_plane.h"
#include "editor/commands/emitters/move_editor_view_limiter_command_emitter.h"
#include "editor/inspector/positionable_selection_pane.h"
#include "node/isometric_map.h"

#include <editor/editor_node.h>
#include <editor/editor_plugin.h>

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
        void refresh(int p_plane_type);

        node::IsometricMap* get_selected_map() const;
        EditorPlane& get_editor_plane_for_selected_map(EditorPlane::PlaneType p_plane_type);

        bool is_aabb_in_view_limiters(const AABB& p_aabb) const;

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
            Ref<SceneTreeTimer> plane_timers[EditorPlane::PlaneType::SIZE];
            bool is_grid[EditorPlane::PlaneType::SIZE];

            MapHandlingData();
            explicit MapHandlingData(const Vector3& p_map_size);
        };

        editor::inspector::PositionableSelectionPane* positionable_selection_pane;
        ToolButton* positionable_pane_button;
        UndoRedo* undo_redo;
        HBoxContainer* toolbar;
        ColorPickerButton* grid_color_picker_button;
        OptionButton* edition_mode_button;
        Button* debug_button;

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
        commands::emitters::MoveEditorViewLimiterCommandEmitter plane_view_limiter_command_emitter;

        void _on_frame_post_draw();

        void _on_edition_mode_changed(int selected_index);

        void _on_plane_visibility_timeout(int p_plane_type);

        void _on_map_size_changed();

        void _on_grid_color_picker_change(const Color& p_color);

        void _draw_edition_grid() const;
        void _draw_plane(EditorPlane::PlaneType p_plane_type);
        void _set_plane_timer(EditorPlane::PlaneType p_plane_type, float p_delay);

    public:
        static void _bind_methods();
    };

}// namespace editor

#endif
#endif
