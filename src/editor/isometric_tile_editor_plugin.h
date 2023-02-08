#ifndef ISOMETRIC_MAPS_ISOMETRIC_TILE_EDITOR_PLUGIN_H
#define ISOMETRIC_MAPS_ISOMETRIC_TILE_EDITOR_PLUGIN_H

#ifdef TOOLS_ENABLED

#include <node/isometric_positionable.h>
#include <scene/gui/color_picker.h>
#include <editor/editor_plugin.h>

namespace editor {
    class IsometricTileEditorPlugin : public EditorPlugin {
        GDCLASS(IsometricTileEditorPlugin, EditorPlugin)

    public:
        static IsometricTileEditorPlugin* get_instance();

        IsometricTileEditorPlugin();
        ~IsometricTileEditorPlugin() override = default;

        IsometricTileEditorPlugin(const IsometricTileEditorPlugin&) = delete;

    protected:
        void _notification(int p_notification);
        bool handles(Object* p_object) const override;
        void edit(Object* p_object) override;
        void make_visible(bool p_visible) override;

    private:
        void _on_size_changed();
        void _on_color_picker_change(const Color& p_color);

        node::IsometricPositionable* selected_positionable;
        HBoxContainer* toolbar;
        ColorPickerButton* color_picker_button;

    public:
        static void _bind_methods();
    };
}// namespace editor

#endif
#endif// ISOMETRIC_MAPS_ISOMETRIC_TILE_EDITOR_PLUGIN_H
