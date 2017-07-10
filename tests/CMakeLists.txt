include_directories(BEFORE ..)

project(test_animated_algebra)

add_executable(test_circle_node test_circle_node.cpp)
target_link_libraries(test_circle_node animated_algebra)
add_test(test_circle_node ${PROJECT_BINARY_DIR}/test_circle_node)
