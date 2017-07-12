include_directories(BEFORE ..)

project(test_animated_algebra)

add_executable(test_algebra test_algebra.cpp)
target_link_libraries(test_algebra animated_algebra)
add_test(test_algebra ${PROJECT_BINARY_DIR}/test_algebra)