include_directories(BEFORE ..)

project(test_liven)

add_executable(test_matrix test_matrix.cpp)
target_link_libraries(test_matrix liven)
add_test(test_matrix ${PROJECT_BINARY_DIR}/test_matrix)

add_executable(test_image test_image.cpp)
target_link_libraries(test_image liven)
add_test(test_image ${PROJECT_BINARY_DIR}/test_image)

add_executable(test_graphics test_graphics.cpp)
target_link_libraries(test_graphics liven)
add_test(test_graphics ${PROJECT_BINARY_DIR}/test_graphics)

add_executable(test_scene test_scene.cpp)
target_link_libraries(test_scene liven)
add_test(test_scene ${PROJECT_BINARY_DIR}/test_scene)

add_executable(test_grid_node test_grid_node.cpp)
target_link_libraries(test_grid_node liven)
add_test(test_grid_node ${PROJECT_BINARY_DIR}/test_grid_node)

add_executable(test_ascii_renderer test_ascii_renderer.cpp)
target_link_libraries(test_ascii_renderer liven)
add_test(test_ascii_renderer ${PROJECT_BINARY_DIR}/test_ascii_renderer)

add_executable(test_text_node test_text_node.cpp)
target_link_libraries(test_text_node liven)
add_test(test_text_node ${PROJECT_BINARY_DIR}/test_text_node)

add_executable(test_arrow_node test_arrow_node.cpp)
target_link_libraries(test_arrow_node liven)
add_test(test_arrow_node ${PROJECT_BINARY_DIR}/test_arrow_node)

add_executable(test_unique_sequence test_unique_sequence.cpp)
target_link_libraries(test_unique_sequence liven)
add_test(test_unique_sequence ${PROJECT_BINARY_DIR}/test_unique_sequence)

add_executable(test_lineup_node test_lineup_node.cpp)
target_link_libraries(test_lineup_node liven)
add_test(test_lineup_node ${PROJECT_BINARY_DIR}/test_lineup_node)

add_executable(test_viewer test_viewer.cpp)
target_link_libraries(test_viewer liven)
add_test(test_viewer ${PROJECT_BINARY_DIR}/test_viewer)
