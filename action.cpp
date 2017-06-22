#include "action.h"
#include "algebra.h"
#include <cmath>
#include <iostream>
#include <typeinfo>

bool action::act(int frame_number) {
	if (frame_number < get_start_frame()) {return true;}
	if (current_frame == 0) {will_start();}
	remaining_action = false;
	if (own_act()) {remaining_action = true;}
	// size_t children_size = children.size();
	for (int i = 0; i < children.size();/*no increment*/) {
		if (children[i]->act(current_frame)) {
			remaining_action = true;
			++i;
		}
		else {
			// the child action is finished
			// call its did_finish function, if it has one
			auto child_did_finish = children[i]->did_finish;
			if (child_did_finish) {child_did_finish(shared_from_this(),children[i]);}
			children.erase(children.begin() + i);
		}
	}
	++current_frame;
	return remaining_action;
}
/*
// sequential_action
bool sequential_action::act(int frame_number) {
	if (action::act(frame_number)) {return true;}
	else if (action_queue.empty()) {return false;}
	else {
		children.push_back(action_queue.front());
		action_queue.pop();
		return true;
	}
}
*/
// move
void move::update_children_frame_count_settings() {
	std::shared_ptr<move> move_child_ptr;
	for (auto child : children) {
		if ((move_child_ptr = std::dynamic_pointer_cast<move>(child))) {
			move_child_ptr->fixed_frame_count = this->fixed_frame_count;
			move_child_ptr->node_speed = this->node_speed;
			move_child_ptr->low_frame_limit = this->low_frame_limit;
			move_child_ptr->high_frame_limit = this->high_frame_limit;
			move_child_ptr->frame_count_method = this->frame_count_method;
			move_child_ptr->internal_update();
			move_child_ptr->update_children_frame_count_settings();
		}
	}
}

void shift::compute_f_count_and_increments() {
	if (frame_count_method == fixed) {f_count = fixed_frame_count;}
	else {
		int d_max = (std::abs(dx) > std::abs(dy)) ? dx : dy;
		if (d_max == 0) {f_count = 0;}
		else {f_count = std::abs(std::ceil((double) d_max / (double) node_speed));}
		if (frame_count_method == limit) {
			if (f_count < low_frame_limit) {
				f_count = low_frame_limit;
			}
			if (f_count > high_frame_limit) {
				f_count = high_frame_limit;
			}
		}
	}

	if (f_count == 0) {
		// debug
		std::cout << "f_count == 0" << std::endl;
		x_increment = 0;
		y_increment = 0;
		return;
	}

	/*
	Suppose we have 60 people and we want to divide them into 13 
	approximately equal-sized groups.  Euclidean division suggests groups
	of size 4.  But then there are 8 people leftover.  Each person from
	the 8 remaining should be assigned to a group of 4, leaving 8 groups
	of 5 and 5 groups of 4.
	Generalizing: let there be n people we wish to divide into g groups,
	and let floor(n / g) == k, so that there are k people in each group
	before assigning the remaining r people to groups.  We then assign
	an addition person to each of r groups.  There are then r groups of 
	k + 1 people and (g - r) groups of k people.  No two groups then differ
	in size by more than 1 person.
	Mathematically, we write n = (k + 1) * r + k * (g - r)
	Here, we will divide dx or dy into f_count increments, no two of
	which differ by more than one coordinate in size.
	dx = (x_increment + 1) * x_r + (x_increment)(f_count - x_r);
	*/

	auto quo_rem = algebra::fdiv_qr(dx,f_count);
	x_increment = std::get<0>(quo_rem);
	x_r = std::get<1>(quo_rem);
	quo_rem = algebra::fdiv_qr(dy,f_count);
	y_increment = std::get<0>(quo_rem);
	y_r = std::get<1>(quo_rem);
}

shift::shift(std::shared_ptr<node> n_, int start_frame_, int dx_, int dy_, frame_count_method_type fcm) : 
			move(n_, start_frame_, fcm), dx(dx_), dy(dy_)
{
	compute_f_count_and_increments();
}

bool shift::own_act() {
	bool remaining_action = true;

	if (current_frame < x_r) {n->shift(x_increment + 1,0);}
	else if (current_frame < f_count) {n->shift(x_increment,0);}

	if (current_frame < y_r) {n->shift(0,y_increment + 1);}
	else if (current_frame < f_count) {n->shift(0,y_increment);}

	if (current_frame >= f_count - 1) {remaining_action = false;}

	// do not update current_frame here.  it is updated in action::act()
	return remaining_action;
}

// space_children
space_children::space_children(std::shared_ptr<node> n_, int start_frame_, int dx_, int dy_, frame_count_method_type fcm) :
								move(n_,start_frame_,fcm), dx(dx_), dy(dy_)
{
	create_children();
}

bool space_children::own_act() {
	return false;
}

void space_children::create_children() {
	children.clear();
	std::shared_ptr<shift> child_shift;
	for (int i = 1; i < n->get_children().size(); ++i) {
		child_shift = std::make_shared<shift>(n->get_children()[i],0,dx * i, dy * i);
		child_shift->fixed_frame_count = this->fixed_frame_count;
		child_shift->node_speed = this->node_speed;
		child_shift->low_frame_limit = this->low_frame_limit;
		child_shift->high_frame_limit = this->high_frame_limit;
		child_shift->use_frame_count_method(frame_count_method);

		children.push_back(child_shift);
	}
}

// settle
settle::settle(std::shared_ptr<node> n_, int start_frame_, dimension dim_,
		int target_coordinate_, frame_count_method_type fcm) :
		move(n_,start_frame_,fcm), dim(dim_), target_coordinate(target_coordinate_) {
	create_children();
}

void settle::create_children() {
	for (auto child_node : n->get_children()) {
		std::shared_ptr<shift> child_shift;
		int delta;

		switch (dim) {
			case dimension::x:
			delta = target_coordinate - child_node->get_location().x;
			child_shift = std::make_shared<shift>(child_node,0,delta,0);
			break;
			case dimension::y:
			delta = target_coordinate - child_node->get_location().y;
			child_shift = std::make_shared<shift>(child_node,0,0,delta);
			break;
		}
		child_shift->use_frame_count_method(this->frame_count_method);
		children.push_back(child_shift);
	}
}
