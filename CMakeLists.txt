cmake_minimum_required(VERSION 3.13)
project(isometric_maps)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(GODOT_ROOT_DIR ../../)

# Get sources
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_library(${PROJECT_NAME} SHARED register_types.cpp ${SOURCES} src/node/isometric_positionable.cpp src/node/isometric_positionable.h src/editor/outline_drawer.cpp src/editor/outline_drawer.h src/positionable/isometric_element.h src/node/isometric_map.cpp src/node/isometric_map.h src/containers/grid_3d.cpp src/containers/grid_3d.h src/physics/default_body.h src/physics/default_kinematic_body.cpp src/physics/default_kinematic_body.h src/physics/default_static_body.cpp src/physics/default_static_body.h src/positionable/isometric_tile.cpp src/positionable/isometric_tile.h src/positionable/isometric_placeholder.cpp src/positionable/isometric_placeholder.h src/resource/placeholder_type.cpp src/resource/placeholder_type.h src/isometric_world.cpp src/isometric_world.h src/resource/isometric_configuration.cpp src/resource/isometric_configuration.h src/variant_casters.h src/data//isometric_positionable_data.cpp src/data//isometric_positionable_data.h src/isometric_server.cpp src/isometric_server.h src/data//isometric_world_data.cpp src/data/isometric_world_data.h)

target_include_directories(${PROJECT_NAME}
        PUBLIC ${GODOT_ROOT_DIR}
)
