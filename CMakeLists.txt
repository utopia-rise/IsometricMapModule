cmake_minimum_required(VERSION 3.13)
project(isometric_maps)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(GODOT_ROOT_DIR ../../)

# Get sources
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_library(${PROJECT_NAME} SHARED register_types.cpp ${SOURCES} src/node/isometric_positionable.cpp src/node/isometric_positionable.h src/editor/outline_drawer.cpp src/editor/outline_drawer.h src/node/isometric_map.cpp src/node/isometric_map.h src/containers/grid_3d.h src/physics/default_body.h src/physics/default_kinematic_body.cpp src/physics/default_kinematic_body.h src/physics/default_static_body.cpp src/physics/default_static_body.h src/resource/placeholder_type.cpp src/resource/placeholder_type.h src/resource/isometric_configuration.cpp src/resource/isometric_configuration.h src/data//isometric_element.cpp src/data//isometric_element.h src/isometric_server.cpp src/isometric_server.h src/data//isometric_space.cpp src/data/isometric_space.h src/data/isometric_parameters.h src/data/isometric_parameters.cpp src/utils/isometric_maths.h src/editor/isometric_editor_plugin.h src/editor/isometric_editor_plugin.cpp src/editor/editor_plane.cpp src/editor/editor_plane.h src/editor/editor_axes.h src/editor/edition_grid_drawer.cpp src/editor/edition_grid_drawer.h src/editor/inspector/positionable_selection_pane.cpp src/editor/inspector/positionable_selection_pane.h src/resource/positionable_set.cpp src/resource/positionable_set.h src/editor/positionable_scenes_cache_manager.h src/editor/positionable_scenes_cache_manager.cpp src/editor/editor_utils.cpp src/editor/editor_utils.h src/editor/positionable_set_editor_plugin.cpp src/editor/positionable_set_editor_plugin.h src/editor/inspector/positionable_set_editor.cpp src/editor/inspector/positionable_set_editor.h src/editor/commands/command.h src/editor/commands/add_positionable_command.cpp src/editor/commands/add_positionable_command.h)

target_include_directories(${PROJECT_NAME} PUBLIC ${GODOT_ROOT_DIR})

# Uncomment to pass auto completion in TOOLS mode
add_compile_definitions(TOOLS_ENABLED)
add_compile_definitions(DEBUG_ENABLED)

# platforms
#add_compile_definitions(WINDOWS_ENABLED)
add_compile_definitions(X11_ENABLED)
#add_compile_definitions(OSX_ENABLED)
#add_compile_definitions(__ANDROID__)