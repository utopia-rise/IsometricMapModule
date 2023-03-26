#include "register_types.h"

#include "isometric_server.h"
#include "node/isometric_map.h"
#include "resource/isometric_configuration.h"

#ifdef TOOLS_ENABLED
#include "editor/isometric_editor_plugin.h"
#include "editor/isometric_tile_editor_plugin.h"
#include "editor/positionable_set_editor_plugin.h"
#endif

#ifdef TOOLS_ENABLED

static EditorPlugin* isometric_editor_plugin_creator_func() {
    return editor::IsometricEditorPlugin::get_instance();
}

static EditorPlugin* positionable_set_editor_plugin_creator_func() {
    return editor::PositionableSetEditorPlugin::get_instance();
}

static EditorPlugin* isometric_tile_editor_plugin_creator_func() {
    return editor::IsometricTileEditorPlugin::get_instance();
}
#endif

void initialize_isometric_maps_module(ModuleInitializationLevel p_level) {
    if (p_level == MODULE_INITIALIZATION_LEVEL_SERVERS) {

    }

    if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
        ClassDB::register_class<resource::IsometricConfiguration>();
        ClassDB::register_class<node::IsometricPositionable>();
        ClassDB::register_class<node::IsometricMap>();
        ClassDB::register_class<resource::PositionableSet>();
        IsometricServer::create_server();
        ClassDB::register_class<IsometricServer>();
        Engine::get_singleton()->add_singleton(
          Engine::Singleton("IsometricServer", reinterpret_cast<Object*>(IsometricServer::get_instance()))
        );
    }

#ifdef TOOLS_ENABLED
    if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
        ClassDB::register_class<editor::IsometricEditorPlugin>();
        ClassDB::register_class<editor::PositionableSetEditorPlugin>();
        ClassDB::register_class<editor::IsometricTileEditorPlugin>();
        EditorPlugins::add_create_func(isometric_editor_plugin_creator_func);
        EditorPlugins::add_create_func(positionable_set_editor_plugin_creator_func);
        EditorPlugins::add_create_func(isometric_tile_editor_plugin_creator_func);
        ClassDB::register_class<editor::inspector::PositionableSelectionPane>();
        ClassDB::register_class<editor::inspector::PositionableSetEditor>();
    }
#endif
}

void uninitialize_isometric_maps_module(ModuleInitializationLevel p_level) {
    if (p_level == MODULE_INITIALIZATION_LEVEL_SERVERS) {

    }
    if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
        IsometricServer::terminate_server();
    }
}
