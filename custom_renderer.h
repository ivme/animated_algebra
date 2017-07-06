#ifndef CUSTOM_RENDERER_H
#define CUSTOM_RENDERER_H

#include "liven/default_renderer.h"
#include "circle_node.h"

using liven::default_renderer;

class custom_renderer {
public:
	using image_type = default_renderer::image_type;
	template<class T>
	static image_type render(const T &t) {return default_renderer::render(t);}
};

template<>
custom_renderer::image_type custom_renderer::render(const circle_node &cn) {std::cout << "circle with radius " << cn.radius << std::endl; return image_type();}

#endif