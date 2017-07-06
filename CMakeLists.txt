cmake_minimum_required (VERSION 2.6)
project (animated_algebra)
# The version number.
set (ANIMATED_ALGEBRA_MAJOR_VERSION 0)
set (ANIMATED_ALGEBRA_MINOR_VERSION 1)

add_definitions(-DPRIVACY_OFF)
enable_testing()
add_compile_options(
    -std=c++11
    -g
)

include_directories(BEFORE ./liven)

set(AA_SRC )

# add_library(animated_algebra ${AA_SRC})

add_subdirectory(liven)

