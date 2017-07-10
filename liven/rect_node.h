#ifndef RECT_NODE_H
#define RECT_NODE_H

#include "grid_node.h"

namespace liven {

class rect_node : public node {

public:
	// when we construct a rect_node, we should also
	// add the grid as a child of the rect_node
	// but this cannot be done in the constructor
	// because adding the child makes a weak_ptr to
	// this, which causes a runtime error when done
	// in the constructor.

	// therefore privatize the constructor and 
	// use a public, static pseudo-constructor function
	// `make` to construct rect_nodes.
	static std::shared_ptr<rect_node> make(int width_, int height_);

	rect_node(rect r) : 
		rect_node(r.width,r.height) {}

	int get_width() const;
	int get_height() const;
	void set_width(int width_);
	void set_height(int height_);

	std::shared_ptr<grid_node> get_grid() const;

private:
	rect_node(int width_, int height_) :
		node(),
		width(width_),
		height(height_),
		grid(std::make_shared<grid_node>(
			width_,
			height_,
			std::vector<int>({0,width_}),
			std::vector<int>({0,height_})))
	{}

	std::shared_ptr<grid_node> grid;
	int width, height;

	void update_grid();
};

} // liven

#endif