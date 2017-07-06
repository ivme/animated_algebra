#ifndef GLOBAL_DEFS_H
#define GLOBAL_DEFS_H

#define Q(x) #x
#define QUOTE(x) Q(x)

#define LIVEN_DEFAULT_RENDER_TYPE liven::ascii_renderer
#define LIVEN_DEFAULT_RENDER_TYPE_HEADER_PATH ascii_renderer.h

// if the user compiling liven has specified a renderer
// and a path to the header containing the declaration
// of the renderer, use it.  otherwise use the default
// renderer.

#ifdef LIVEN_RENDER_TYPE
	#ifndef LIVEN_RENDER_TYPE_HEADER_PATH
		#error Render class specified, but path to render class header not specified.
	#else
	#include QUOTE(LIVEN_RENDER_TYPE_HEADER_PATH)
	#endif
#else
#define LIVEN_RENDER_TYPE LIVEN_DEFAULT_RENDER_TYPE
#include QUOTE(LIVEN_DEFAULT_RENDER_TYPE_HEADER_PATH)
#endif

#include <cmath>
#include "graphics.h"

namespace liven {
	using render_type = LIVEN_RENDER_TYPE;
	using image_type = render_type::image_type;

template<class RENDER_TYPE = render_type>
located<rect,2> scene_rect_to_pixel_rect(located<rect,2> scene_rect) {
	int x = std::floor(RENDER_TYPE::scene_x_coordinate_to_pixels(scene_rect.location.x));
	int y = std::floor(RENDER_TYPE::scene_y_coordinate_to_pixels(scene_rect.location.y));
	int width = std::ceil(RENDER_TYPE::scene_x_coordinate_to_pixels(scene_rect.width));
	int height = std::ceil(RENDER_TYPE::scene_y_coordinate_to_pixels(scene_rect.height));
	return located<rect,2>(rect(width,height),point<2>(x,y));
}

}

#endif