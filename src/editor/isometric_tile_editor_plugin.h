#ifndef ISOMETRIC_MAPS_ISOMETRIC_TILE_EDITOR_PLUGIN_H
#define ISOMETRIC_MAPS_ISOMETRIC_TILE_EDITOR_PLUGIN_H

#ifdef TOOLS_ENABLED

#include "node/isometric_positionable.h"

#include <editor/editor_plugin.h>

namespace editor {
    class IsometricTileEditorPlugin : public EditorPlugin {
        GDCLASS(IsometricTileEditorPlugin, EditorPlugin)

    public:
        static IsometricTileEditorPlugin* get_instance();

        IsometricTileEditorPlugin();
        ~IsometricTileEditorPlugin() override = default;

        IsometricTileEditorPlugin(const IsometricTileEditorPlugin&) = delete;

        static void _bind_methods();

    protected:
        bool handles(Object* p_object) const override;
        void edit(Object* p_object) override;

    private:
        void _on_size_changed();

        node::IsometricPositionable* selected_positionable;
    };
}// namespace editor

#endif
#endif// ISOMETRIC_MAPS_ISOMETRIC_TILE_EDITOR_PLUGIN_H
