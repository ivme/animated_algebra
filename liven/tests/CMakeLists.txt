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