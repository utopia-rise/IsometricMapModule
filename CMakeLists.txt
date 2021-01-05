cmake_minimum_required(VERSION 3.13)
project(isometric_maps)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(GODOT_ROOT_DIR ../../)

# Get sources
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_library(${PROJECT_NAME} SHARED register_types.cpp ${SOURCES} src/positionable/isometric_positionable.cpp src/positionable/isometric_positionable.h src/editor/outline_drawer.cpp src/editor/outline_drawer.h src/positionable/isometric_element.h src/positionable/isometric_map.cpp src/positionable/isometric_map.h src/containers/grid_3d.cpp src/containers/grid_3d.h src/positionable/dynamic_isometric_element.cpp src/positionable/dynamic_isometric_element.h src/positionable/physics/default_body.h src/positionable/physics/default_kinematic_body.cpp src/positionable/physics/default_kinematic_body.h src/positionable/static_isometric_element.cpp src/positionable/static_isometric_element.h src/positionable/physics/default_static_body.cpp src/positionable/physics/default_static_body.h src/positionable/isometric_tile.cpp src/positionable/isometric_tile.h src/positionable/isometric_placeholder.cpp src/positionable/isometric_placeholder.h src/positionable/placeholder_type.cpp src/positionable/placeholder_type.h src/isometric_world.cpp src/isometric_world.h src/isometric_api.cpp src/isometric_api.h)

target_include_directories(${PROJECT_NAME}
        PUBLIC ${GODOT_ROOT_DIR}
)
