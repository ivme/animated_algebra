set(SRC 
	default_renderer.cpp
)

add_library(liven ${SRC})

IF(BUILD_LIVEN_TESTS)
add_subdirectory(tests)
ENDIF(BUILD_LIVEN_TESTS)