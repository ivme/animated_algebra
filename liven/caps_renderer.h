#ifndef CAPS_RENDERER_H
#define CAPS_RENDERER_H

#include "node_declarations.h"
#include "ascii_image.h"
#include "default_renderer.h"

namespace liven {

struct caps_renderer {
	using image_type = ascii_image;
	template<class T>
	static image_type render(const T &t) {default_renderer::render(t); return ascii_image();}
};

// explicitly specialize caps_renderer to render text_nodes in all caps	
template<>
caps_renderer::image_type caps_renderer::render<text_node>(const text_node &tn) {
	std::string upper_text = tn.text;
	for (auto &c : upper_text) {c = std::toupper(c);}
	std::cout << upper_text << std::endl;
	return ascii_image();
}

}


#endif