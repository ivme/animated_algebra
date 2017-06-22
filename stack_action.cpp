#include "stack_action.h"
#include <iostream> // debug

stack_action::stack_action(std::shared_ptr<node> n_, int start_frame_, dimension dim_, int initial_spacing_, int final_spacing_) :
							action(n_,start_frame_), dim(dim_), initial_spacing(initial_spacing_), final_spacing(final_spacing_) {}

void stack_action::will_start() {
	compute_deltas();
	create_children();
}

void stack_action::compute_deltas() {
	located<rect,2> bounding_rect = n->get_children()[0]->bounding_rect();
	switch (dim) {
		case dimension::x:
		dx1 = 0;
		dy1 = initial_spacing;
		dx2 = bounding_rect.width - 1 + final_spacing;
		dy2 = 0;
		settle_dimension = dimension::y;
		break;
		case dimension::y:
		dx1 = initial_spacing;
		dy1 = 0;
		dx2 = 0;
		dy2 = bounding_rect.height - 1 + final_spacing;
		settle_dimension = dimension::x;
		break;
	}
}

void stack_action::create_children() {
	int dx2_ = this->dx2;
	int dy2_ = this->dy2;
	auto settle_dimension_ = this->settle_dimension;
	action::callback_func add_settle_down = [settle_dimension_](std::shared_ptr<action> parent, std::shared_ptr<action> finished_action) {
		auto settle_down = std::make_shared<settle>(finished_action->get_node(),parent->get_current_frame() + 1,settle_dimension_);
		parent->add_child(settle_down);
	};

	action::callback_func add_space_children2 = [add_settle_down,dx2_,dy2_](std::shared_ptr<action> parent, std::shared_ptr<action> finished_action) {
		auto space_children2 = std::make_shared<space_children>(finished_action->get_node(),parent->get_current_frame(),dx2_,dy2_);
		space_children2->did_finish = add_settle_down;
		parent->add_child(space_children2);
	};

	auto space_children1 = std::make_shared<space_children>(n,0,dx1,dy1);
	space_children1->did_finish = add_space_children2;
	if (initial_spacing != 0) {
		add_child(space_children1);
	} else {
		add_space_children2(shared_from_this(),space_children1);
	}
	
}
