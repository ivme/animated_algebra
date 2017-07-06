#ifndef LIMITED_RENDERER_H
#define LIMITED_RENDERER_H

#include "liven/default_renderer.h"
#include "circle_node.h"

using liven::default_renderer;

class limited_renderer {
public:
	using image_type = default_renderer::image_type;
	static image_type render(const circle_node &cn) {std::cout << "circle with radius " << cn.radius << std::endl; return image_type();}
};
#endif