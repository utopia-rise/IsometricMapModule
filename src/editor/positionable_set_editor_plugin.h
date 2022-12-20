#ifndef ISOMETRIC_MAPS_POSITIONABLE_SET_EDITOR_PLUGIN_H
#define ISOMETRIC_MAPS_POSITIONABLE_SET_EDITOR_PLUGIN_H

#ifdef TOOLS_ENABLED
#include "editor/inspector/positionable_set_editor.h"

#include <editor/editor_plugin.h>

namespace editor {
    class PositionableSetEditorPlugin : public EditorPlugin {
        GDCLASS(PositionableSetEditorPlugin, EditorPlugin)

    public:
        static PositionableSetEditorPlugin* get_instance();

        PositionableSetEditorPlugin();
        ~PositionableSetEditorPlugin() override = default;

        PositionableSetEditorPlugin(const PositionableSetEditorPlugin&) = delete;

    protected:
        void _notification(int p_notification);
        bool handles(Object* p_object) const override;
        void make_visible(bool b) override;
        void edit(Object* p_object) override;

    private:
        inspector::PositionableSetEditor* positionable_set_editor;
        ToolButton* positionable_set_editor_button;

    public:
        static void _bind_methods();
    };
}// namespace editor

#endif
#endif// ISOMETRIC_MAPS_POSITIONABLE_SET_EDITOR_PLUGIN_H
