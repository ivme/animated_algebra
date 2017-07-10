#ifndef DEFAULT_RENDERER_H
#define DEFAULT_RENDERER_H

#include "ascii_image.h"
#include <iostream>

namespace liven {

class rect_node;
class text_node;

class default_renderer {
public:
	using image_type = ascii_image;
	static image_type render(const rect_node &rn);
	static image_type render(const text_node &tn);
};


}

#endif
