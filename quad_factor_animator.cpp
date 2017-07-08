#include "quad_factor_animator.h"
#include "p_rect.h"
#include "liven/action.h"
#include "liven/stack_action.h"
#include "algebra.h"
#include <typeinfo> //debug
#include <cassert> //debug

using liven::scene;
using liven::node;
using liven::text_node;
using liven::point;
using liven::move;
using liven::animation;
using liven::dimension;
using liven::shift;
using liven::action;
using liven::stack_action;

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
	cols = {col1,col2,col3};
	for (auto pr : cols) {pr->set_display_style(p_rect::display_style_type::center_expanded);}
	col1_node = std::make_shared<node>(col1);
	col2_node = std::make_shared<node>(col2);
	col3_node = std::make_shared<node>(col3);
	col2_node->set_location(col1->width() + p_rect::unit_size,0,0);
	col3_node->set_location(col1->width() + col2->width() + 2 * p_rect::unit_size,0,0);
	p_rect_nodes = std::make_shared<node>();
	p_rect_nodes->add_child(col1_node);
	p_rect_nodes->add_child(col2_node);
	p_rect_nodes->add_child(col3_node);
	v.scn->get_root()->add_child(p_rect_nodes);
	p_rect_nodes->shift(p_rect::unit_size, p_rect::unit_size);

	// text_node for the quadratic expression
	quad_expr = std::make_shared<text_node>();
	quad_expr->set_text(algebra::sum_to_string({{"x^2",a},{"x",b},{"1",c}}));
	quad_expr_node = std::make_shared<node>(quad_expr);
	v.scn->get_root()->add_child(quad_expr_node);
	quad_expr->align_left(*quad_expr_node);
	quad_expr->set_anchor(*quad_expr_node, point<2>(0,0));
}

void quad_factor_animator::apply_move_settings(std::shared_ptr<move> n) {
	n->use_frame_count_method(fcm);
	n->set_fixed_frame_count(fixed_frame_count);
	n->set_node_speed(node_speed);
	n->set_low_frame_limit(low_frame_limit);
	n->set_high_frame_limit(high_frame_limit);
}

std::shared_ptr<animation> quad_factor_animator::animate() {
	animation_ = std::make_shared<animation>();
	int unit_size = p_rect::unit_size;
	
	set_up_scene();
	assert(p_rect_nodes->get_children().size() == 3);

	pause(2.5);
	for (auto pr : cols) {pr->set_display_style(p_rect::display_style_type::labels_and_lines);}
	pause(1.0);

	// factors ax^2 + bx + c into (a0x + b0)(a1x + b1)
	std::tuple<bool,int,int,int,int> f = algebra::quad_factor(a,b,c);

	bool is_factorable = std::get<0>(f);
	if (!is_factorable) {throw std::runtime_error("not factorable");}
	int a0 = std::get<1>(f);
	int b0 = std::get<2>(f);
	int a1 = std::get<3>(f);
	int b1 = std::get<4>(f);

	// split first column into a0 columns, each of which is 1x wide and a1 x's high (group 1)
	auto col1_split_response = col1->split(col1_node,dimension::y,a0);
	group1 = col1_split_response.parent;
	#ifndef NDEBUG
	assert(p_rect_nodes->get_children().size() == 3);
	assert(group1->get_children().size() == a0);
	#endif

	// first split second column into two sub-columns
	// col 2a:  1x wide by a0*b1 1's high
	// col 2b:  1x wide by a1*b0 1's high
	// call the parent of these two sub-columns group2
	std::set<int> split_points = {a0 * b1};
	auto col2_split_response = col2->split(col2_node,dimension::y,split_points);
	group2 = col2_split_response.parent;
	col2a_node = col2_split_response.children_wrapped.begin()->first;
	col2b_node = (++col2_split_response.children_wrapped.begin())->first;
	col2a = col2_split_response.children_wrapped[col2a_node];
	col2b = col2_split_response.children_wrapped[col2b_node];
	#ifndef NDEBUG
	assert(p_rect_nodes->get_children().size() == 3);
	assert(group2->get_children().size() == 2);
	assert(col2a->x_lengths.size() == 1);
	assert(col2a->y_lengths.size() == a0 * b1);
	assert(col2b->x_lengths.size() == 1);
	assert(col2b->y_lengths.size() == a1*b0);
	#endif

	// split 3rd column into b0 p_rects, each of which is 1 wide and b1 1's high (group 3)
	// set parent of resulting collection of p_rects to group3
	auto col3_split_response = col3->split(col3_node,dimension::y,b0);
	group3 = col3_split_response.parent;
	#ifndef NDEBUG
	assert(p_rect_nodes->get_children().size() == 3);
	assert(group3->get_children().size() == b0);
	if (b0 > 0) {
		auto tmp_rect = col3_split_response.children_wrapped.begin()->second;
		assert(tmp_rect->x_lengths.size() == 1);
		assert(tmp_rect->y_lengths.size() == b1);
	}
	#endif

	// shift group 3 to allow space for 
	// stacking group 2.
	// auto shift_3 = std::make_shared<shift>(group3,1,unit_size + var_val * unit_size,0);
	// shift_3->use_frame_count_method(fcm);
	render_action(make_move_action<shift>(group3,1,unit_size + var_val * unit_size,0));
	#ifndef NDEBUG
	assert(p_rect_nodes->get_children().size() == 3);
	assert(group3->get_scene_location().x > group2->get_scene_location().x);
	#endif

	//	stack_horizontal group2 (since its children are group2a and group2b,
	//	only those groups will separate and stack)
	//		space group2's children vertically
	//		space group2's children horizontally
	//		settle group2's children down
	auto stack_2 = make_move_action<stack_action>(group2,0,dimension::x,unit_size / 2,unit_size);
	render_action(stack_2);
	#ifndef NDEBUG
	assert(p_rect_nodes->get_children().size() == 3);
	assert(col2a_node->get_scene_location().x < col2b_node->get_scene_location().x);
	#endif

	// briefly show the expanded display of
	// col2a and col2b
	flash_display_style({col2a,col2b},p_rect::display_style_type::center_expanded,.5,2.0,.5);

	// split subcolumns 2a and 2b into two groups of columns:
	// group 2a: a0 columns, each of which is 1x wide and b1 1's high
	// group 2b: a1 columns, each of which is 1x wide and b0 1's high
	auto col2a_split_response = col2a->split(col2a_node, dimension::y,a0);
	group2a = col2a_split_response.parent;
	#ifndef NDEBUG
	assert(p_rect_nodes->get_children().size() == 3);
	assert(group2->get_children().size() == 2);
	assert(group2a->get_children().size() == a0);
	if (a0 > 0) {
		auto tmp_rect = col2a_split_response.children_wrapped.begin()->second;
		assert(tmp_rect->x_lengths.size() == 1);
		assert(tmp_rect->y_lengths.size() == b1);
	}
	#endif
	auto col2b_split_response = col2b->split(col2b_node, dimension::y,a1);
	group2b = col2b_split_response.parent;
	#ifndef NDEBUG
	assert(p_rect_nodes->get_children().size() == 3);
	assert(group2->get_children().size() == 2);
	assert(group2b->get_children().size() == a1);
	if (a1 > 0) {
		auto tmp_rect = col2b_split_response.children_wrapped.begin()->second;
		assert(tmp_rect->x_lengths.size() == 1);
		assert(tmp_rect->y_lengths.size() == b0);
	}
	#endif

	if (b0 > 1) {
		// stack_horizontal group3
		// space group3's children vertically
		// space group3's children horizontally
		// settle group3's children down

		render_action(make_move_action<stack_action>(group3,0,dimension::x,unit_size / 2,0));
	}

	//debug
	std::cout << "group3->get_scene_location().x == " << group3->get_scene_location().x << std::endl;

	// merge group3 back into a single p_rect
	auto group3_merge_response = p_rect::merge(
		p_rect_nodes,
		col3_split_response.children_wrapped,
		dimension::y);
	auto rect3_node = group3_merge_response.n;
	auto rect3 = group3_merge_response.pr;
	#ifndef NDEBUG
	assert(rect3_node->get_parent().lock() == p_rect_nodes);
	assert(rect3_node->get_children().size() == 0);
	assert(rect3->x_lengths.size() == b0);  // should be b0 columns, each 1 wide
	assert(rect3->y_lengths.size() == b1);  // should be b1 rows, each 1 high
	#endif

	if (a1 > 1) {
		// shift rect3 right to make space for stacking of group2b
		render_action(make_move_action<shift>(rect3_node,0, (a1 - 1) * var_val * unit_size,0));

		// stack group2b horizontally
		render_action(make_move_action<stack_action>(group2b,0,dimension::x,unit_size / 2,0));
	}

	// merge group2b into a single p_rect
	auto group2b_merge_response = p_rect::merge(
		group2,
		col2b_split_response.children_wrapped,
		dimension::y);
	auto rect2b_node = group2b_merge_response.n;
	auto rect2b = group2b_merge_response.pr;
	#ifndef NDEBUG
	assert(rect2b_node->get_parent().lock() == group2);
	assert(rect2b->x_lengths.size() == a1);
	assert(rect2b->y_lengths.size() == b0);
	#endif

	// if necessary, shift rect3_node to make space
	// for swapping group2b's x's and y's
	int height_2b = rect2b->height();
	int width_2b = rect2b->width();
	if (height_2b > width_2b) {
		render_action(make_move_action<shift>(rect3_node,0,height_2b - width_2b,0));
	}

	// pause the animation for half a second
	// to clarify the swap visually 
	pause(.5);
	// swap x lengths and y lengths for group2b
	rect2b->swap_x_y();
	pause(.5);
	#ifndef NDEBUG
	std::cout << "rect2b_node->get_scene_location().x == " << rect2b_node->get_scene_location().x << std::endl;
	std::cout << "rect3_node->get_scene_location().x == " << rect3_node->get_scene_location().x << std::endl;
	assert(rect2b_node->get_parent().lock() == group2);
	assert(rect2b->x_lengths.size() == b0);
	assert(rect2b->y_lengths.size() == a1);
	#endif
	
	// group 2b and 3 under a common parent, preserving scene locations
	auto group_2b_3 = std::make_shared<node>();
	v.scn->get_root()->add_child(group_2b_3); // must be added to the scene for preserve_scene_location to work
	group_2b_3->set_location(rect2b_node->get_scene_location());
	p_rect_nodes->add_child(group_2b_3,true);
	group_2b_3->add_child(rect2b_node,true);
	group_2b_3->add_child(rect3_node,true);

	// debug
	std::cout << "rect2b_node->get_scene_location().x == " << rect2b_node->get_scene_location().x << std::endl;
	std::cout << "rect3_node->get_scene_location().x == " << rect3_node->get_scene_location().x << std::endl;

	// group 1 and 2a under a common parent, preserving scene locations
	auto group_1_2a = std::make_shared<node>();
	v.scn->get_root()->add_child(group_1_2a);
	group_1_2a->set_location(group1->get_scene_location());
	p_rect_nodes->add_child(group_1_2a,true);
	group_1_2a->add_child(group1,true);
	group_1_2a->add_child(group2a,true);

	// group2 no longer has any children.  remove it from p_rect_nodes.
	p_rect_nodes->remove_child(group2);

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

	// debug
	std::cout << "rect2b_node->get_scene_location().x == " << rect2b_node->get_scene_location().x << std::endl;
	std::cout << "rect3_node->get_scene_location().x == " << rect3_node->get_scene_location().x << std::endl;

	// merge group1
	auto group1_merge_response = p_rect::merge(
		group_1_2a,
		col1_split_response.children_wrapped,
		dimension::y);
	auto rect1_node = group1_merge_response.n;
	auto rect1 = group1_merge_response.pr;

	// merge group2a
	auto group2a_merge_response = p_rect::merge(
		group_1_2a,
		col2a_split_response.children_wrapped,
		dimension::y);
	auto rect2a_node = group2a_merge_response.n;
	auto rect2a = group2a_merge_response.pr;
	

	flash_display_style({rect1,rect2a},p_rect::display_style_type::center_expanded,.5,2.0,.5);

	// stack group_1_2a vertically
	render_action(make_move_action<stack_action>(group_1_2a,0,dimension::y));

	auto group_1_2a_merge_response = p_rect::merge(
		p_rect_nodes,
		{{rect1_node,rect1},{rect2a_node,rect2a}},
		dimension::x);
	auto rect_1_2a_node = group_1_2a_merge_response.n;
	auto rect_1_2a = group_1_2a_merge_response.pr;

	flash_display_style({rect_1_2a},p_rect::display_style_type::center_factored,.5,2.0,.5);
	
	flash_display_style({rect2b,rect3},p_rect::display_style_type::center_expanded,.5,2.0,.5);
	// stack group_2b_3 vertically
	render_action(make_move_action<stack_action>(group_2b_3,0,dimension::y));

	// debug
	std::cout << "rect2b_node->get_scene_location().x == " << rect2b_node->get_scene_location().x << std::endl;
	std::cout << "rect3_node->get_scene_location().x == " << rect3_node->get_scene_location().x << std::endl;

	auto group_2b_3_merge_response = p_rect::merge(
		p_rect_nodes,
		{{rect2b_node,rect2b},{rect3_node,rect3}},
		dimension::x);
	auto rect_2b_3_node = group_2b_3_merge_response.n;
	auto rect_2b_3 = group_2b_3_merge_response.pr;
	#ifndef NDEBUG
	assert(rect_2b_3_node->get_parent().lock() == p_rect_nodes);
	assert(rect_2b_3->x_lengths.size() == b0);
	assert(rect_2b_3->y_lengths.size() == a1 + b1);
	assert(rect_2b_3->x_lengths[0].name == "1");
	assert(rect_2b_3->x_lengths[0].val == 1);
	assert(rect_2b_3->y_lengths[0].name == var_name);
	assert(rect_2b_3->y_lengths[0].val = var_val);
	#endif

	flash_display_style({rect_2b_3},p_rect::display_style_type::center_factored,.5,2.0,.5);
	
	// join left and right
	render_action(make_move_action<shift>(rect_2b_3_node,0,-(group_2b_3->get_location().x - group_1_2a->bounding_rect().width + 1),0));

	auto rect_all_merge_response = p_rect::merge(
		p_rect_nodes,
		{{rect_1_2a_node,rect_1_2a},{rect_2b_3_node,rect_2b_3}},
		dimension::y);
	auto rect_all_node = rect_all_merge_response.n;
	auto rect_all = rect_all_merge_response.pr;

	flash_display_style({rect_all},p_rect::display_style_type::center_factored,.5,2.0,.5);

	return animation_;
}

void quad_factor_animator::flash_display_style(
		std::vector<std::shared_ptr<p_rect>> p_rects,
		p_rect::display_style_type ds,
		double pause_before,
		double pause_during,
		double pause_after)
{
	std::vector<p_rect::display_style_type> styles{};
	for (auto pr : p_rects) {
		styles.push_back(pr->get_display_style());
	}
	pause(pause_before);
	for (auto pr : p_rects) {
		pr->set_display_style(ds);	
	}
	pause(pause_during);
	for (int i = 0; i < p_rects.size(); ++i) {
		p_rects[i]->set_display_style(styles[i]);
	}
	pause(pause_after);
}




