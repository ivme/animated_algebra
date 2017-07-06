include_directories(BEFORE ..)

project(test_liven)

add_executable(test_node test_node.cpp)
add_test(test_node ${PROJECT_BINARY_DIR}/test_node)
