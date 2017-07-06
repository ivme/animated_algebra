set(SRC
	action.cpp
	animation.cpp
	animator.cpp
	graphics.cpp
	image.cpp
	matrix.cpp
	node.cpp
	ascii_renderer.cpp
	scene.cpp
	stack_action.cpp
	text_node.cpp
	grid_node.cpp
	ascii_viewer.cpp
	rect_node.cpp
)

add_library(liven ${SRC})

add_subdirectory(tests)