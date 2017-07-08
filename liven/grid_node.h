#ifndef GRID_NODE_H
#define GRID_NODE_H

#include "scene.h"

namespace liven {

class grid_node {
public:
	grid_node() : grid_node(0,0,{},{}) {}
	grid_node(int width_, int height_, std::vector<int> v_partitions_, std::vector<int> h_paritions_) : 
				width(width_), height(height_), v_partitions(v_partitions_), h_partitions(h_paritions_) {
					if (width < 0 || height < 0) {throw std::runtime_error("negative grid_node dimension");}

					/* Since scene coordinates represent a platonic ideal of 
					   the scene rather than any rendering of the scene, each partition
					   is strictly one dimensional.  Vertical partitions, for example,
					   occupy zero horizontal space in scene coordinates.  Therefore, a grid
					   with a width of 2 units should be able to accommodate 3 vertical
					   partitions, located at x=0, x=1 and x=2.
					*/
					for (int i : v_partitions) {if (i > width) {throw std::runtime_error("vertical partition larger than width");}}
					for (int i : h_partitions) {if (i > height) {throw std::runtime_error("horizontal partition larger than height");}}
				}
	
	int width;
	int height;
	std::vector<int> v_partitions;
	std::vector<int> h_partitions;
};

}

#endif