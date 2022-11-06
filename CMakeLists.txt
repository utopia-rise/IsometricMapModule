cmake_minimum_required(VERSION 3.13)
project(isometric_maps)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(GODOT_ROOT_DIR ../../)

# Get sources
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_library(${PROJECT_NAME} SHARED register_types.cpp ${SOURCES} src/node/isometric_positionable.cpp src/node/isometric_positionable.h src/editor/outline_drawer.cpp src/editor/outline_drawer.h src/node/isometric_map.cpp src/node/isometric_map.h src/containers/grid_3d.h src/resource/placeholder_type.cpp src/resource/placeholder_type.h src/resource/isometric_configuration.cpp src/resource/isometric_configuration.h src/data/isometric_element.h src/isometric_server.cpp src/isometric_server.h src/data/isometric_space.h src/data/isometric_parameters.h src/data/isometric_parameters.cpp src/utils/isometric_maths.h src/editor/isometric_editor_plugin.h src/editor/isometric_editor_plugin.cpp src/editor/editor_plane.cpp src/editor/editor_plane.h src/editor/edition_grid_drawer.cpp src/editor/edition_grid_drawer.h src/editor/inspector/positionable_selection_pane.cpp src/editor/inspector/positionable_selection_pane.h src/resource/positionable_set.cpp src/resource/positionable_set.h src/editor/positionable_scenes_cache_manager.h src/editor/positionable_scenes_cache_manager.cpp src/editor/editor_utils.cpp src/editor/editor_utils.h src/editor/positionable_set_editor_plugin.cpp src/editor/positionable_set_editor_plugin.h src/editor/inspector/positionable_set_editor.cpp src/editor/inspector/positionable_set_editor.h src/editor/commands/command.h src/editor/commands/add_positionable_command.cpp src/editor/commands/add_positionable_command.h src/logging.h src/editor/commands/emitters/command_emitter.h src/editor/commands/emitters/painting_command_emitter.cpp src/editor/commands/emitters/painting_command_emitter.h src/editor/positionable_selector_manager.cpp src/editor/positionable_selector_manager.h src/editor/commands/select_positionable_command.cpp src/editor/commands/select_positionable_command.h src/editor/commands/emitters/select_command_emitter.cpp src/editor/commands/emitters/select_command_emitter.h src/editor/commands/command.cpp src/editor/commands/emitters/delete_command_emitter.cpp src/editor/commands/emitters/delete_command_emitter.h src/editor/commands/revert_command.cpp src/editor/commands/revert_command.h src/editor/commands/composite_command.cpp src/editor/commands/composite_command.h src/editor/commands/emitters/drag_and_drop_command_emitter.cpp src/editor/commands/emitters/drag_and_drop_command_emitter.h src/editor/commands/emitters/select_all_command_emitter.cpp src/editor/commands/emitters/select_all_command_emitter.h  src/editor/commands/move_editor_plane_command.cpp src/editor/commands/move_editor_plane_command.h src/editor/commands/emitters/move_editor_grid_command_emitter.cpp src/editor/commands/emitters/move_editor_grid_command_emitter.h src/editor/outline_data.cpp src/editor/outline_data.h src/editor/outline_data.cpp src/editor/outline_data.h src/editor/isometric_tile_editor_plugin.cpp src/editor/isometric_tile_editor_plugin.h src/editor/commands/rotate_editor_plane_command.cpp src/editor/commands/rotate_editor_plane_command.h src/editor/commands/emitters/rotate_editor_plane_command_emitter.cpp src/editor/commands/emitters/rotate_editor_plane_command_emitter.h src/editor/commands/emitters/move_editor_view_limiter_command_emitter.h src/editor/commands/emitters/move_editor_view_limiter_command_emitter.cpp src/editor/commands/set_positionable_visibility_command.cpp src/editor/commands/set_positionable_visibility_command.h)

target_include_directories(${PROJECT_NAME} PUBLIC ${GODOT_ROOT_DIR})

# Uncomment to pass auto completion in TOOLS mode
add_compile_definitions(TOOLS_ENABLED)
add_compile_definitions(DEBUG_ENABLED)

# platforms
#add_compile_definitions(WINDOWS_ENABLED)
#add_compile_definitions(X11_ENABLED)
add_compile_definitions(OSX_ENABLED)
add_compile_definitions(APPLE_STYLE_KEYS)
#add_compile_definitions(__ANDROID__)