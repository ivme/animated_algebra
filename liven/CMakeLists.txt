set(SRC
	action.cpp
	animator.cpp
	graphics.cpp
	matrix.cpp
	ascii_renderer.cpp
	scene.cpp
	stack_action.cpp
	text_node.cpp
	viewer.cpp
	node.cpp
	lineup_node.cpp
)

add_library(liven ${SRC})

add_subdirectory(tests)