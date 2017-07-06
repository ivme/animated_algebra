cmake_minimum_required (VERSION 2.6)
project (animated_algebra)
# The version number.
set (ANIMATED_ALGEBRA_MAJOR_VERSION 0)
set (ANIMATED_ALGEBRA_MINOR_VERSION 1)

add_definitions(-DPRIVACY_OFF)
enable_testing()
add_compile_options(
    -std=c++14
    -g
)

include_directories(BEFORE ./liven)

set(AA_SRC
	aa_controller.cpp
	algebra.cpp
	p_rect.cpp
	quad_factor_animator.cpp
)

add_library(animated_algebra ${AA_SRC})

add_executable(main main.cpp)
target_link_libraries(main animated_algebra liven)

add_subdirectory(liven)

