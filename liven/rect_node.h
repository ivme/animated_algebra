#ifndef RECT_NODE_H
#define RECT_NODE_H

#include "grid_node.h"

namespace liven {

class rect_node : public node {

public:
	rect_node(rect r) : 
		width(r.width), height(r.height) {}

	int get_width() const;
	int get_height() const;
	void set_width(int width_);
	void set_height(int height_);

	grid_node get_grid() const;

private:
	int width, height;
};

}

#endif