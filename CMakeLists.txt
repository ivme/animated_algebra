cmake_minimum_required (VERSION 2.6)
project (animated_algebra)
# The version number.
set (ANIMATED_ALGEBRA_MAJOR_VERSION 0)
set (ANIMATED_ALGEBRA_MINOR_VERSION 1)

set(RENDER_TYPE aa_renderer)
set(RENDER_TYPE_HEADER_PATH ${PROJECT_SOURCE_DIR}/aa_renderer.h)

add_definitions(-DPRIVACY_OFF -DLIVEN_RENDER_TYPE=${RENDER_TYPE} -DLIVEN_RENDER_TYPE_HEADER_PATH=${RENDER_TYPE_HEADER_PATH})
enable_testing()
add_compile_options(
    -std=c++11
    -g
)

set(AA_SRC
	aa_controller.cpp
	aa_renderer.cpp
	algebra.cpp
	p_rect.cpp
	quad_factor_animator.cpp
)

add_library(animated_algebra ${AA_SRC})

add_executable(main main.cpp)
target_link_libraries(main animated_algebra liven)

add_subdirectory(liven)

