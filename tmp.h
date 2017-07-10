#ifndef AA_RENDERER_H
#define AA_RENDERER_H

#include "liven/ascii_renderer.h"
#include "liven/grid_node.h"

class aa_renderer {
public:
	using image_type = ascii_renderer::image_type;
	template <class T>
	static image_type render(const T &t) {return ascii_renderer::render(t);}
}

template<>
aa_renderer::image_type aa_renderer::render<p_rect>(const p_rect &pr) {
	
}

#endif