#include "rect_node.h"

using namespace liven;

int rect_node::get_width() const {return width;}
int rect_node::get_height() const {return height;}
void rect_node::set_width(int width_) {width = width_;}
void rect_node::set_height(int height_) {height = height_;}

grid_node rect_node::get_grid() const {
	return grid_node(width,
					height,
					std::vector<int>({0,width}),
					std::vector<int>({0,height})
	);
}