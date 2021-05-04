#ifndef ISOMETRIC_MAPS_ISOMETRIC_EDITOR_PLUGIN_H
#define ISOMETRIC_MAPS_ISOMETRIC_EDITOR_PLUGIN_H

#include <modules/isometric_maps/src/node/isometric_map.h>
#include "editor/editor_node.h"
#include "editor/editor_plugin.h"

namespace editor {

    class IsometricEditorPlugin : public EditorPlugin {

        enum Mode {
            NONE,
            SELECT,
            PAIN
        };

        IsometricEditorPlugin();

        node::IsometricMap* selected_map;

        Mode current_mode;

    public:
        static IsometricEditorPlugin* get_instance();

        IsometricEditorPlugin(const IsometricEditorPlugin&) = delete;

        IsometricEditorPlugin& operator=(const IsometricEditorPlugin&) = delete;

    protected:
        void _notificationv(int p_notification, bool p_reversed) override;

        bool forward_canvas_gui_input(const Ref<InputEvent>& p_event) override;

        void edit(Object* p_object) override;

        bool handles(Object* p_object) const override;

        void clear() override;

    public:
        static void _bind_methods();
    };

}

#endif
