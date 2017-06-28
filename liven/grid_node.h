#ifndef GRID_NODE_H
#define GRID_NODE_H

#include "scene.h"

class grid_node : public node {
public:
	grid_node(int width_, int height_, std::vector<int> v_partitions_, std::vector<int> h_paritions_) : 
				node(), width(width_), height(height_), v_partitions(v_partitions_), h_partitions(h_paritions_) {}
	
	int width;
	int height;
	std::vector<int> v_partitions;
	std::vector<int> h_partitions;

	virtual bool is_renderable() const {return true;}
	virtual std::shared_ptr<ascii_image> render(renderer<ascii_image> &r);
	virtual std::shared_ptr<ascii_image> render(ascii_renderer &r);

};

#endif