#include "custom_renderer.h"
#include "circle_node.h"

template<>
custom_renderer::image_type custom_renderer::render(const circle_node &cn) {
	std::cout << "circle with radius " << cn.radius << std::endl; return image_type();
	return image_type();
}