#ifndef ISOMETRIC_MAPS_ISOMETRIC_TILE_EDITOR_PLUGIN_H
#define ISOMETRIC_MAPS_ISOMETRIC_TILE_EDITOR_PLUGIN_H

#ifdef TOOLS_ENABLED

    #include <editor/editor_plugin.h>

namespace editor {
    class IsometricTileEditorPlugin : public EditorPlugin {
        GDCLASS(IsometricTileEditorPlugin, EditorPlugin)

    public:
        static IsometricTileEditorPlugin* get_instance();

        IsometricTileEditorPlugin() = default;
        ~IsometricTileEditorPlugin() override = default;

        IsometricTileEditorPlugin(const IsometricTileEditorPlugin&) = delete;

    protected:
        bool handles(Object* p_object) const override;
        void edit(Object* p_object) override;
    };
}// namespace editor

#endif
#endif// ISOMETRIC_MAPS_ISOMETRIC_TILE_EDITOR_PLUGIN_H
