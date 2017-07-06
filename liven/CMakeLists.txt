set(SRC
	action.cpp
	animation.cpp
	animator.cpp
	graphics.cpp
	image.cpp
	matrix.cpp
	node.cpp
	renderer.cpp
	scene.cpp
	stack_action.cpp
	text_node.cpp
	grid_node.cpp
	viewer.cpp
	rect_node.cpp
)

add_library(liven ${SRC})

add_subdirectory(tests)