#include "register_types.h"
#include <core/engine.h>
#include <modules/isometric_maps/src/isometric_server.h>
#include <modules/isometric_maps/src/node/isometric_map.h>
#include <modules/isometric_maps/src/resource/isometric_configuration.h>

#ifdef TOOLS_ENABLED
#include <modules/isometric_maps/src/editor/isometric_editor_plugin.h>
#include <editor/editor_node.h>
#include <modules/isometric_maps/src/resource/positionable_set.h>
#include <modules/isometric_maps/src/editor/positionable_set_editor_plugin.h>

#endif

#ifdef TOOLS_ENABLED

static EditorPlugin* isometric_editor_plugin_creator_func(EditorNode* editor_node) {
    return editor::IsometricEditorPlugin::get_instance();
}

static EditorPlugin* positionable_set_editor_plugin_creator_func(EditorNode* editor_node) {
    return editor::PositionableSetEditorPlugin::get_instance();
}
#endif

void register_isometric_maps_types() {
    ClassDB::register_class<IsometricServer>();
    Engine::get_singleton()->add_singleton(Engine::Singleton("IsometricServer", reinterpret_cast<Object *>(IsometricServer::get_instance())));

    ClassDB::register_class<resource::IsometricConfiguration>();

    ClassDB::register_class<node::IsometricPositionable>();
    ClassDB::register_class<node::IsometricMap>();

    ClassDB::register_class<editor::OutlineDrawer>();

    ClassDB::register_class<resource::PositionableSet>();

    #ifdef TOOLS_ENABLED
        ClassDB::register_class<editor::IsometricEditorPlugin>();
        ClassDB::register_class<editor::PositionableSetEditorPlugin>();
        EditorPlugins::add_create_func(isometric_editor_plugin_creator_func);
        EditorPlugins::add_create_func(positionable_set_editor_plugin_creator_func);
        ClassDB::register_class<editor::inspector::PositionableSelectionPane>();
        ClassDB::register_class<editor::inspector::PositionableSetEditor>();
    #endif

}

void unregister_isometric_maps_types() {

}
