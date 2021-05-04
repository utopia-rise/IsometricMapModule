#ifndef ISOMETRIC_MAPS_ISOMETRIC_EDITOR_PLUGIN_H
#define ISOMETRIC_MAPS_ISOMETRIC_EDITOR_PLUGIN_H

#include "editor/editor_node.h"
#include "editor/editor_plugin.h"

namespace editor{

    class IsometricEditorPlugin : public EditorPlugin {

        IsometricEditorPlugin();

    public:
        static IsometricEditorPlugin* get_instance();

        IsometricEditorPlugin(const IsometricEditorPlugin&) = delete;

        IsometricEditorPlugin& operator=(const IsometricEditorPlugin&) = delete;

    protected:
        void _notificationv(int p_notification, bool p_reversed) override;

    public:
        static void _bind_methods();
    };

}

#endif
