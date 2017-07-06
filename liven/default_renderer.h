#ifndef DEFAULT_RENDERER_H
#define DEFAULT_RENDERER_H

#include "node_declarations.h"
#include "ascii_image.h"

namespace liven {

class default_renderer {
public:
	using image_type = ascii_image;
	static image_type render(const rect_node &rn) {std::cout << rn.width << "," << rn.height << std::endl; return ascii_image();}
	static image_type render(const text_node &tn) {std::cout << tn.text << std::endl; return ascii_image();}
};


}

#endif
