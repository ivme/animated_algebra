#include "quad_factor_animator.h"
#include "p_rect.h"
#include "action.h"
#include "stack_action.h"
#include "algebra.h"
#include <typeinfo> //debug
#include <cassert> //debug

quad_factor_animator::quad_factor_animator(int a_, int b_, int c_) : animator(),
	a(a_),
	b(b_),
	c(c_), 
	var_name(default_var_name),
	var_val(default_var_val)
{
	// set_up_scene();
	// set_up_actions();
}

void quad_factor_animator::set_up_scene() {
	// set up scene
	// 3 p_rects to start
	// 1st column: 1x by a x's
	// 2nd column: 1x by b 1's
	// 3rd column: 1 by c 1's

	v.scn = std::make_shared<scene>();
	auto r = std::make_shared<node>();
	v.scn->set_root(r);

	length x{var_name,var_val};
	length unit_length{};
	col1 = std::make_shared<p_rect>(std::vector<length>(1,x),std::vector<length>(a,x));
	col2 = std::make_shared<p_rect>(std::vector<length>(1,x),std::vector<length>(b,unit_length));
	col3 = std::make_shared<p_rect>(std::vector<length>(1,unit_length),std::vector<length>(c,unit_length));
	col2->set_location(col1->width() + p_rect::unit_size,0,0);
	col3->set_location(col1->width() + col2->width() + 2 * p_rect::unit_size,0,0);
	v.scn->get_root()->add_child(col1);
	v.scn->get_root()->add_child(col2);
	v.scn->get_root()->add_child(col3);
}

void quad_factor_animator::apply_move_settings(std::shared_ptr<move> n) {
	n->use_frame_count_method(fcm);
	n->set_fixed_frame_count(fixed_frame_count);
	n->set_node_speed(node_speed);
	n->set_low_frame_limit(low_frame_limit);
	n->set_high_frame_limit(high_frame_limit);
}

std::shared_ptr<animation<wchar_t>> quad_factor_animator::animate() {
	set_up_scene();

	animation_ = std::make_shared<animation<wchar_t>>();
	int unit_size = p_rect::unit_size;

	// factors ax^2 + bx + c into (a0x + b0)(a1x + b1)
	std::tuple<bool,int,int,int,int> f = algebra::quad_factor(a,b,c);

	bool is_factorable = std::get<0>(f);
	if (!is_factorable) {throw std::runtime_error("not factorable");}
	int a0 = std::get<1>(f);
	int b0 = std::get<2>(f);
	int a1 = std::get<3>(f);
	int b1 = std::get<4>(f);

	// split first column into a0 columns, each of which is 1x wide and a1 x's high (group 1)
	group1 = col1->split(dimension::y,a0);

	// first split second column into two sub-columns
	// col 2a:  1x wide by a0*b1 1's high
	// col 2b:  1x wide by a1*b0 1's high
	// call the parent of these two sub-columns group2
	std::set<int> split_points = {a0 * b1};
	group2 = col2->split(dimension::y,split_points);
	col2a = std::dynamic_pointer_cast<p_rect>(group2->get_children()[0]);
	col2b = std::dynamic_pointer_cast<p_rect>(group2->get_children()[1]);

	// split subcolumns 2a and 2b into two groups of columns:
	// group 2a: a0 columns, each of which is 1x wide and b1 1's high
	// group 2b: a1 columns, each of which is 1x wide and b0 1's high
	group2a = col2a->split(dimension::y,a0);
	group2b = col2b->split(dimension::y,a1);

	// split 3rd column into b0 p_rects, each of which is 1 wide and b1 1's high (group 3)
	// set parent of resulting collection of p_rects to group3
	group3 = col3->split(dimension::y,b0);

	// shift group 3 to allow space for 
	// stacking group 2.
	// auto shift_3 = std::make_shared<shift>(group3,1,unit_size + var_val * unit_size,0);
	// shift_3->use_frame_count_method(fcm);
	render_action(make_move_action<shift>(group3,1,unit_size + var_val * unit_size,0));

	//	stack_horizontal group2 (since its children are group2a and group2b,
	//	only those groups will separate and stack)
	//		space group2's children vertically
	//		space group2's children horizontally
	//		settle group2's children down
	auto stack_2 = make_move_action<stack_action>(group2,0,dimension::x,unit_size / 2,unit_size);

	render_action(stack_2);

	if (b0 > 1) {
		// stack_horizontal group3
		// space group3's children vertically
		// space group3's children horizontally
		// settle group3's children down

		render_action(make_move_action<stack_action>(group3,0,dimension::x,unit_size / 2,0));
	}

	if (a1 > 1) {
		// shift group3 right to make space for stacking of group2b
		render_action(make_move_action<shift>(group3,0, (a1 - 1) * var_val * unit_size,0));

		// stack group2b horizontally
		render_action(make_move_action<stack_action>(group2b,0,dimension::x,unit_size / 2,0));
	}

	// merge group2b into a single p_rect
	auto rect2b = p_rect::merge(group2b, dimension::y);

	// if necessary, shift group3 to make space
	// for swapping group2b's x's and y's
	int height_2b = rect2b->height();
	int width_2b = rect2b->width();
	if (height_2b > width_2b) {
		render_action(make_move_action<shift>(group3,0,height_2b - width_2b,0));
	}

	// pause the animation for half a second
	// to clarify the swap visually 
	pause(animation_->get_animation_speed() / 2);
	// swap x lengths and y lengths for group2b
	rect2b->swap_x_y();
	pause(animation_->get_animation_speed() / 2);
	
	// group 2b and 3 under a common parent, preserving scene locations
	auto group_2b_3 = std::make_shared<node>();
	group_2b_3->set_location(rect2b->get_scene_location());
	v.scn->get_root()->add_child(group_2b_3,true);
	group_2b_3->add_child(rect2b,true);
	group_2b_3->add_child(group3,true);

	// group 1 and 2a under a common parent, preserving scene locations
	auto group_1_2a = std::make_shared<node>();
	v.scn->get_root()->add_child(group_1_2a,true);
	group_1_2a->add_child(group1,true);
	group_1_2a->add_child(group2a,true);

	if (a0 > 1) {
		// shift group2b and group3 right to make room for stacking of group2a
		render_action(make_move_action<shift>(group_2b_3,0,(a0 - 1) * var_val * unit_size,0));

		// stack group2a horizontally
		render_action(make_move_action<stack_action>(group2a,0,dimension::x,unit_size));
	}
	
	if (a0 > 1) {
		// shift group2a, group 2b, and group3 right
		// to make room for stacking of group1
		auto shift_2b_3 = make_move_action<shift>(group_2b_3,0,(a0 - 1) * var_val * unit_size, 0);
		auto shift_2a = make_move_action<shift>(group2a,0,(a0 - 1) * var_val * unit_size, 0);
		auto shift_2a_2b_3 = std::make_shared<action>();
		shift_2a_2b_3->add_child(shift_2a);
		shift_2a_2b_3->add_child(shift_2b_3);
		render_action(shift_2a_2b_3);

		// stack group1 horizontally
		render_action(make_move_action<stack_action>(group1,0,dimension::x,unit_size));
	}

	// stack group_1_2a vertically
	render_action(make_move_action<stack_action>(group_1_2a,0,dimension::y));
	
	// stack group_2b_3 vertically
	render_action(make_move_action<stack_action>(group_2b_3,0,dimension::y));
	
	// join left and right
	render_action(make_move_action<shift>(group_2b_3,0,-(group_2b_3->get_location().x - group_1_2a->bounding_rect().width + 1),0));

	return animation_;
}



