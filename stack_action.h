#ifndef STACK_ACTION_H
#define STACK_ACTION_H

#include "action.h"

namespace liven {

class stack_action : public move {
	// this action assumes that the child nodes of 
	// n_ are already stacked either horizontally or
	// vertically
	// it it also follows space_children's assumption that
	// children are sorted by x coordinate or
	// y coordinate, respectively
	// finally, it assumes that all children are
	// the same size.
	// dimension = x : stack horizontally
	// dimension = y : stack vertically

	// NEW
	// for ease of explanation, choose dimmenion::x
	// assumptions:
	// (1) the p_rects are of equal width
	// (2) the p_rects are already stacked vertically.  this means:
	// 		(a) the x-coordinates of the p_rects are the same
	//		(b) the p_rects are sorted by y-coordinate
	// (3) the children of n_ are all p_rects
	// sort the child nodes by y-coordinate?  (or assume sorted?)
	// then increases the vertical spacing between nodes by initial_spacing
	// assume the nodes are of equal width?
	// assu

public:
	stack_action(std::shared_ptr<node> parent_,
		std::shared_ptr<node> zero_child_node,
		std::shared_ptr<p_rect> zero_childe_rect,
		dimension dim = dimension::x,
		int initial_spacing = 0,
		int final_spacing = 0);
	virtual void will_start() override;

#ifndef PRIVACY_OFF
private:
#endif
	void compute_deltas();
	void create_children();
	dimension dim;
	int initial_spacing;
	int final_spacing;

	int dx1;
	int dy1;
	int dx2;
	int dy2;
	dimension settle_dimension;
};

}

#endif