#include "default_renderer.h"
#include "node.h"

using liven::default_renderer;

default_renderer::image_type default_renderer::render(const rect_node &rn) {std::cout << rn.width << "," << rn.height << std::endl; return ascii_image();}
default_renderer::image_type default_renderer::render(const text_node &tn) {std::cout << tn.text << std::endl; return ascii_image();}