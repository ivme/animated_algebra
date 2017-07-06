#include "rect_node.h"

using namespace liven;

std::shared_ptr<rect_node> rect_node::make(int width_, int height_) {
	auto out = std::shared_ptr<rect_node>(new rect_node(width_,height_));
	out->add_child(out->get_grid());
	return out;
}

void rect_node::update_grid() {
	remove_child(grid);
	add_child(std::make_shared<grid_node>(
			width,
			height,
			std::vector<int>({0,width}),
			std::vector<int>({0,height})
			)
	);
}

int rect_node::get_width() const {return width;}
int rect_node::get_height() const {return height;}
void rect_node::set_width(int width_) {width = width_; update_grid();}
void rect_node::set_height(int height_) {height = height_; update_grid();}

std::shared_ptr<grid_node> rect_node::get_grid() const {return grid;}