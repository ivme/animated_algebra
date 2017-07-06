#ifndef NODE_DECLARATIONS_H
#define NODE_DECLARATIONS_H

namespace liven {

struct rect_node {
	rect_node(int w, int h) : width(w), height(h) {}
	int width;
	int height;
};

struct text_node {
	text_node(std::string text_) : text(text_) {}
	std::string text;
};

}

#endif