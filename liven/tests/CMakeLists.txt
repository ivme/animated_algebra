include_directories(BEFORE ..)

project(test_liven)

add_executable(test_node test_node.cpp)
target_link_libraries(test_node liven)
add_test(test_node ${PROJECT_BINARY_DIR}/test_node)
