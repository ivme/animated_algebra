#include "quad_factor_animator.h"
#include "aa_renderer.h"
#include "p_rect.h"
#include "liven/action.h"
#include "liven/stack_action.h"
#include "liven/arrow_node.h"
#include "algebra.h"
#include <typeinfo> //debug
#include <cassert> //debug

using liven::scene;
using liven::view;
using liven::viewer;
using liven::node;
using liven::located;
using liven::dimension;
using liven::ascii_image;
using liven::ascii_viewer;
using liven::point;
using liven::rect;
using liven::action;
using liven::stack_action;
using liven::shift;
using liven::animation;
using liven::move;
using liven::text_node;
using liven::lineup_node;
using liven::arrow_node;
using liven::unique_sequence;

double brief_pause = 0.0;

int v_scene_coord(int pixels) {
	return pixels / aa_renderer::scene_y_coordinate_to_pixels(1);
}

int h_scene_coord(int pixels) {
	return pixels / aa_renderer::scene_x_coordinate_to_pixels(1);
}

quad_factor_animator::quad_factor_animator(int a_, int b_, int c_) : animator(),
	a(a_),
	b(b_),
	c(c_), 
	var_name(default_var_name),
	var_val(default_var_val)
{}

void quad_factor_animator::set_up_scene() {
	animation_ = animation<ascii_image>();
	animation_.set_animation_speed(10);

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
	for (auto col : {col1, col2, col3}) {col->set_display_style(p_rect::display_style_type::center_expanded);}
	p_rects = std::make_shared<node>();
	v.scn->get_root()->add_child(p_rects);
	p_rects->shift(p_rect::unit_size, v_scene_coord(5)); // 4 lines from bottom

	// text_node for the quadratic expression
	quad_expr = std::make_shared<lineup_node>(dimension::x);
	ax2 = std::make_shared<text_node>(algebra::term_to_string(a,var_name + "^2"));
	p1 = std::make_shared<text_node>(" + ");
	bx1 = std::make_shared<text_node>(algebra::term_to_string(b,var_name));
	p2 = std::make_shared<text_node>(" + ");
	cx0 = std::make_shared<text_node>(algebra::term_to_string(c,""));
	v.scn->get_root()->add_child(quad_expr);
	quad_expr->set_location(point<2>(p_rect::unit_size,v_scene_coord(3)));

	quad_expr->push_back(ax2);
	pause(brief_pause);

	p_rects->add_child(col1);
	pause(brief_pause);
	col1->set_display_style(p_rect::display_style_type::labels_and_lines);
	if (a > 1) {pause(brief_pause);}

	quad_expr->push_back(p1);
	quad_expr->push_back(bx1);
	pause(brief_pause);

	p_rects->add_child(col2);
	pause(brief_pause);
	col2->set_display_style(p_rect::display_style_type::labels_and_lines);
	if (b > 1) {pause(brief_pause);}

	quad_expr->push_back(p2);
	quad_expr->push_back(cx0);
	pause(brief_pause);

	p_rects->add_child(col3);
	pause(brief_pause);
	col3->set_display_style(p_rect::display_style_type::labels_and_lines);
	if (c > 1) {pause(brief_pause);}
}

void quad_factor_animator::apply_move_settings(std::shared_ptr<move> n) {
	n->use_frame_count_method(fcm);
	n->set_fixed_frame_count(fixed_frame_count);
	n->set_node_speed(node_speed);
	n->set_low_frame_limit(low_frame_limit);
	n->set_high_frame_limit(high_frame_limit);
}

animation<ascii_image> quad_factor_animator::animate() {
	// factor ax^2 + bx + c into (a0x + b0)(a1x + b1)
	std::tuple<bool,int,int,int,int> f = algebra::quad_factor(a,b,c);

	bool is_factorable = std::get<0>(f);
	if (!is_factorable) {throw std::runtime_error("not factorable");}
	int a0 = std::get<1>(f);
	int b0 = std::get<2>(f);
	int a1 = std::get<3>(f);
	int b1 = std::get<4>(f);

	int unit_size = p_rect::unit_size;
	
	set_up_scene();

	auto arrow_to_bx1 = std::make_shared<arrow_node>(arrow_node::direction::up, v_scene_coord(1));
	bx1->add_child(arrow_to_bx1);
	arrow_to_bx1->set_anchor(point<2>(0,0));
	arrow_to_bx1->set_anchor_style(arrow_node::anchor_style_type::tip);
	a0b1x = std::make_shared<text_node>(algebra::term_to_string(a0 * b1, var_name));
	a1b0x = std::make_shared<text_node>(algebra::term_to_string(a1 * b0, var_name));
	p3 = std::make_shared<text_node>(" + ");

	auto a0b1x_p3_a1b0x = std::make_shared<lineup_node>(dimension::x);
	quad_expr->add_child(a0b1x_p3_a1b0x);
	a0b1x_p3_a1b0x->set_lineup({a0b1x, p3, a1b0x});
	a0b1x_p3_a1b0x->set_location(point<3>(bx1->get_location().x,v_scene_coord(-2),0));

	// first split second column into two sub-columns
	// col 2a:  1x wide by a0*b1 1's high
	// col 2b:  1x wide by a1*b0 1's high
	// call the parent of these two sub-columns group2
	std::set<int> split_points = {a0 * b1};
	group2 = col2->split(dimension::y,split_points);
	col2a = std::dynamic_pointer_cast<p_rect>(group2->get_children()[0]);
	col2b = std::dynamic_pointer_cast<p_rect>(group2->get_children()[1]);
	p_rect::set_children_display_style(group2,p_rect::display_style_type::center_expanded);
	pause(brief_pause);

	// remove bx
	quad_expr->remove(cx0);
	quad_expr->remove(p2);
	quad_expr->remove(bx1);
	auto p2_cx0 = std::make_shared<lineup_node>(dimension::x);
	quad_expr->add_child(p2_cx0);
	p2_cx0->set_location(p2->get_location());
	p2_cx0->push_back(p2); // p2 is no longer in lineup, but retains its position
	p2_cx0->push_back(cx0); // cx0 is no longer in the lineup, but retains its old position
	pause(brief_pause);

	// shift cx0 to make room for a0b1x_p3_a1b0x in the middle
	render_action(make_move_action<shift>(
		p2_cx0,
		0,
		a0b1x_p3_a1b0x->bounding_rect().width - bx1->own_bounding_rect().width,
		0));

	// move a0b1x_p3_a1b0x into line with rest of quad_expr
	render_action(make_move_action<shift>(
		a0b1x_p3_a1b0x,
		0,
		0,
		-a0b1x_p3_a1b0x->get_location().y));

	// make it all one lineup
	quad_expr->splice(quad_expr->end(), a0b1x_p3_a1b0x);
	quad_expr->splice(quad_expr->end(), p2_cx0);
	quad_expr->remove(a0b1x_p3_a1b0x);
	quad_expr->remove(p2_cx0);
	// quad_expr : ax2,p1,a0b1x,p3,a1b0x,p2,cx0,
	// i.e. ax2 + a0b1x + a1b0x + cx0

	// shift col3 to allow space for 
	// stacking group 2.
	render_action(make_move_action<shift>(col3,1,unit_size + var_val * unit_size,0));

	//	stack_horizontal group2 (since its children are group2a and group2b,
	//	only those groups will separate and stack)
	//		space group2's children vertically
	//		space group2's children horizontally
	//		settle group2's children down
	auto stack_2 = make_move_action<stack_action>(group2,0,dimension::x,unit_size / 2,unit_size);
	render_action(stack_2);

	p_rect::set_children_display_style(group2,p_rect::display_style_type::labels_and_lines);
	pause(.5);

	// show b0b1 below cx0 w/arrow
	auto arrow_to_cx0 = std::make_shared<arrow_node>(arrow_node::direction::up, v_scene_coord(1));
	cx0->add_child(arrow_to_cx0);
	arrow_to_cx0->set_anchor(point<2>(0,0));
	arrow_to_cx0->set_anchor_style(arrow_node::anchor_style_type::tip);

	auto b0b1 = std::make_shared<lineup_node>(dimension::x);
	auto b0_term_4 = std::make_shared<text_node>(std::to_string(b0));
	auto times_term_4 = std::make_shared<text_node>("*");
	auto b1_term_4 = std::make_shared<text_node>(std::to_string(b1));
	b0b1->set_lineup(unique_sequence<std::shared_ptr<node>>({b0_term_4,times_term_4,b1_term_4}));
	quad_expr->add_child(b0b1);
	b0b1->set_location(cx0->get_location().x,v_scene_coord(-2),0);
	pause(brief_pause);

	// split 3rd column into b0 p_rects, each of which is 1 wide and b1 1's high (group 3)
	// set parent of resulting collection of p_rects to group3
	group3 = col3->split(dimension::y,b0);
	p_rect::set_children_display_style(group3,p_rect::display_style_type::center_expanded);
	pause(brief_pause);

	// remove cx0 and move b0b1 into line with the rest of quad_expr
	quad_expr->remove(cx0);
	render_action(make_move_action<shift>(b0b1,0,0,-b0b1->get_location().y));
	quad_expr->insert(quad_expr->end(),b0b1);

	if (b0 > 1) {
		// stack_horizontal group3
		// space group3's children vertically
		// space group3's children horizontally
		// settle group3's children down

		render_action(make_move_action<stack_action>(group3,0,dimension::x,unit_size / 2,0));
	}
	// merge group3 into a single p_rect
	auto rect3 = p_rect::merge(group3, dimension::y);
	rect3->set_display_style(p_rect::display_style_type::labels_and_lines);

	// rewrite a1b0x in long factored form, below a1b0x, with arrow
	auto f_a1b0x = std::make_shared<lineup_node>(dimension::x);
	auto a1_term_3 = std::make_shared<text_node>(std::to_string(a1));
	auto times_term_3 = std::make_shared<text_node>("*");
	auto b0_term_3 = std::make_shared<text_node>(std::to_string(b0));
	auto x_term_3 = std::make_shared<text_node>(var_name);
	f_a1b0x->set_lineup(unique_sequence<std::shared_ptr<node>>({a1_term_3,times_term_3,b0_term_3,x_term_3}));
	quad_expr->add_child(f_a1b0x);
	f_a1b0x->set_location(a1b0x->get_location().x,v_scene_coord(-2),0);

	auto arrow_to_a1b0x = std::make_shared<arrow_node>(arrow_node::direction::up, v_scene_coord(1));
	a1b0x->add_child(arrow_to_a1b0x);
	arrow_to_a1b0x->set_anchor(point<2>(0,0));
	arrow_to_a1b0x->set_anchor_style(arrow_node::anchor_style_type::tip);

	// split column 2b into a group of subcolumns
	// group 2b: a1 columns, each of which is 1x wide and b0 1's high
	group2b = col2b->split(dimension::y,a1);
	p_rect::set_children_display_style(group2b,p_rect::display_style_type::center_expanded);
	pause(brief_pause);

	// remove b0b1
	quad_expr->remove(b0b1);
	quad_expr->remove(p2);
	quad_expr->remove(a1b0x);
	auto p2_b0b1 = std::make_shared<lineup_node>(dimension::x);
	quad_expr->add_child(p2_b0b1);
	p2_b0b1->set_location(p2->get_location());
	p2_b0b1->push_back(p2); 
	p2_b0b1->splice(p2_b0b1->end(),b0b1);
	pause(brief_pause);

	// shift p2_b0b1 to make room for f_a1b0x in the middle
	render_action(make_move_action<shift>(
		p2_b0b1,
		0,
		f_a1b0x->bounding_rect().width - a1b0x->own_bounding_rect().width,
		0));

	// move f_a1b0x into line with rest of quad_expr
	render_action(make_move_action<shift>(
		f_a1b0x,
		0,
		0,
		-f_a1b0x->get_location().y));
	quad_expr->splice(quad_expr->end(),f_a1b0x);
	quad_expr->splice(quad_expr->end(),p2_b0b1);


	if (a1 > 1) {
		// shift rect3 right to make space for stacking of group2b
		render_action(make_move_action<shift>(rect3,0, (a1 - 1) * var_val * unit_size,0));

		// stack group2b horizontally
		render_action(make_move_action<stack_action>(group2b,0,dimension::x,unit_size / 2,0));
	}

	// merge group2b into a single p_rect
	auto rect2b = p_rect::merge(group2b, dimension::y);
	rect2b->set_display_style(p_rect::display_style_type::labels_and_lines);
	pause(.5);

	// if necessary, shift rect3 to make space
	// for swapping group2b's x's and y's
	int height_2b = rect2b->height();
	int width_2b = rect2b->width();
	if (height_2b > width_2b) {
		render_action(make_move_action<shift>(rect3,0,height_2b - width_2b,0));
	}

	// swap a1 and b0 in term 3
	auto split_up_down = std::make_shared<action>();
	split_up_down->add_child(make_move_action<shift>(a1_term_3,0,0,v_scene_coord(-1)));
	split_up_down->add_child(make_move_action<shift>(b0_term_3,0,0,v_scene_coord(1)));
	render_action(split_up_down);
	auto swap_left_right = std::make_shared<action>();
	int left_right_delta = a1_term_3->own_bounding_rect().width + times_term_3->own_bounding_rect().width;
	swap_left_right->add_child(make_move_action<shift>(a1_term_3,0,left_right_delta,0));
	swap_left_right->add_child(make_move_action<shift>(b0_term_3,0,-left_right_delta,0));
	render_action(swap_left_right);
	auto converge_up_down = std::make_shared<action>();
	converge_up_down->add_child(make_move_action<shift>(a1_term_3,0,0,-a1_term_3->get_location().y));
	converge_up_down->add_child(make_move_action<shift>(b0_term_3,0,0,-b0_term_3->get_location().y));
	render_action(converge_up_down);

	// swap x lengths and y lengths for group2b
	rect2b->swap_x_y();
	pause(.5);

	// factor b0 out of both f_a1b0x and b0b1 in the text line
	// need a clean way of removing items from a lineup with altering
	// the positions of the other items.
	// maybe just move them independently of the lineup?
	// the lineup is unaware of the movements of its children
	// until update_locations() is called.
	times_term_3->is_visible = false;
	times_term_4->is_visible = false;
	auto double_drop = std::make_shared<action>();
	double_drop->add_child(make_move_action<shift>(b0_term_3,0,0,v_scene_coord(-1)));
	double_drop->add_child(make_move_action<shift>(b0_term_4,0,0,v_scene_coord(-1)));
	render_action(double_drop);

	// the switcheroo shifts the b0 strings left, to immediately
	// right of p3, so they lie on top of each other.
	// it also shifts the remainder of term 3 and term 4 to the 
	// right and condenses them (spaces were left by the removal
	// of the b0's and the times symbols).
	// it leaves a space for a left paren between b0 and a1 of
	// the third term.
	// e.g.
	// before double_drop
	// 5x^2 + 3x + 5*2x + 2*3
	// 
	// after double_drop, before switcheroo
	// 5x^2 + 3x + 5  x +   3
	//               2    2
	//
	// after switcheroo
	// 5x^2 + 3x +   5x + 3
	//             2
	//
	// after moving b0 (2, in this case) up
	// 5x^2 + 3x + 2 5x + 3
	//
	// after adding parentheses
	// 5x^2 + 3x + 2(5x + 3)

	int b0_term_3_delta = a1_term_3->get_location().x - b0_term_3->get_location().x;
	int b0_term_4_delta = a1_term_3->get_location().x - b0_term_4->get_location().x;
	int a1_term_3_delta = b0_term_3->own_bounding_rect().width + h_scene_coord(1);
	int x_term_3_delta = a1_term_3->get_location().x + a1_term_3_delta + x_term_3->own_bounding_rect().width - x_term_3->get_location().x;
	int p2_delta = x_term_3_delta;
	int b1_term_4_delta = p2->get_location().x + p2_delta + p2->own_bounding_rect().width - b1_term_4->get_location().x;
	auto switcheroo = std::make_shared<action>();
	switcheroo->add_child(make_move_action<shift>(b0_term_3,0,b0_term_3_delta,0));
	switcheroo->add_child(make_move_action<shift>(b0_term_4,0,b0_term_4_delta,0));
	switcheroo->add_child(make_move_action<shift>(a1_term_3,0,a1_term_3_delta,0));
	switcheroo->add_child(make_move_action<shift>(x_term_3,0,x_term_3_delta,0));
	switcheroo->add_child(make_move_action<shift>(p2,0,p2_delta,0));
	switcheroo->add_child(make_move_action<shift>(b1_term_4,0,b1_term_4_delta,0));
	render_action(switcheroo);

	quad_expr->remove_child(b0_term_4);
	render_action(make_move_action<shift>(b0_term_3,0,0,-b0_term_3->get_location().y));
	auto left_paren_group_2 = std::make_shared<text_node>("(");
	auto right_paren_group_2 = std::make_shared<text_node>(")");
	quad_expr->set_lineup(unique_sequence<std::shared_ptr<node>>(
		{ax2,p1,a0b1x,p3,b0_term_3,left_paren_group_2,a1_term_3,x_term_3,p2,b1_term_4,right_paren_group_2}
	));
	pause(brief_pause);

	
	// group 2b and 3 under a common parent, preserving scene locations
	auto group_2b_3 = std::make_shared<node>();
	v.scn->get_root()->add_child(group_2b_3); // must be added to the scene for preserve_scene_location to work
	group_2b_3->set_location(rect2b->get_scene_location());
	p_rects->add_child(group_2b_3,true);
	group_2b_3->add_child(rect2b,true);
	group_2b_3->add_child(rect3,true);

	// stack group_2b_3 vertically
	render_action(make_move_action<stack_action>(group_2b_3,0,dimension::y));
	auto rect_2b_3 = p_rect::merge(group_2b_3, dimension::x);
	flash_display_style({rect_2b_3},p_rect::display_style_type::center_factored,.5,brief_pause,.5);

	// rewrite a0b1x in factored form:  f_a0b1x
	auto f_a0b1x = std::make_shared<lineup_node>(dimension::x);
	auto a0_term_2 = std::make_shared<text_node>(std::to_string(a0));
	auto times_term_2 = std::make_shared<text_node>("*");
	auto b1_term_2 = std::make_shared<text_node>(std::to_string(b1));
	auto x_term_2 = std::make_shared<text_node>(var_name);
	f_a0b1x->set_lineup(unique_sequence<std::shared_ptr<node>>({a0_term_2,times_term_2,b1_term_2,x_term_2}));
	quad_expr->add_child(f_a0b1x);
	f_a0b1x->set_location(a0b1x->get_location().x,v_scene_coord(-2),0);

	auto arrow_to_a0b1x = std::make_shared<arrow_node>(arrow_node::direction::up, v_scene_coord(1));
	a0b1x->add_child(arrow_to_a0b1x);
	arrow_to_a0b1x->set_anchor(point<2>(0,0));
	arrow_to_a0b1x->set_anchor_style(arrow_node::anchor_style_type::tip);

	// split column 2a into a group of subcolumns
	// group 2a: a0 columns, each of which is 1x wide and b1 1's high
	group2a = col2a->split(dimension::y,a0);

	// remove a0b1x
	// and shift everything after a0b1x to the right
	// to make space for factored version f_a0b1x
	auto after_a0b1x = quad_expr->split(std::find(quad_expr->begin(),quad_expr->end(),p3));
	quad_expr->remove(a0b1x);
	pause(brief_pause);

	// shift after_a0b1x to make room for f_a1b0x in the middle
	render_action(make_move_action<shift>(
		after_a0b1x,
		0,
		f_a0b1x->bounding_rect().width - a0b1x->own_bounding_rect().width,
		0));
	pause(1.0);

	// move f_a0b1x into line with rest of quad_expr
	render_action(make_move_action<shift>(
		f_a0b1x,
		0,
		0,
		-f_a0b1x->get_location().y));
	pause(1.0);

	quad_expr->splice(quad_expr->end(),f_a0b1x);
	quad_expr->splice(quad_expr->end(),after_a0b1x);

	if (a0 > 1) {
		// shift rect2b
		render_action(make_move_action<shift>(rect_2b_3,0,(a0 - 1) * var_val * unit_size,0));

		// stack group2a horizontally
		render_action(make_move_action<stack_action>(group2a,0,dimension::x,unit_size));
	}

	// rewrite ax^2 as a0*a1x^2
	auto f_a0a1x2 = std::make_shared<lineup_node>(dimension::x);
	auto a0_term_1 = std::make_shared<text_node>(std::to_string(a0));
	auto times_term_1 = std::make_shared<text_node>("*");
	auto a1_term_1 = std::make_shared<text_node>(std::to_string(a1));
	auto x2 = std::make_shared<text_node>(var_name + "^2");
	f_a0a1x2->set_lineup(unique_sequence<std::shared_ptr<node>>({a0_term_1,times_term_1,a1_term_1,x2}));
	quad_expr->add_child(f_a0a1x2);
	f_a0a1x2->set_location(ax2->get_location().x,v_scene_coord(-2),0);

	auto arrow_to_a0a1x2 = std::make_shared<arrow_node>(arrow_node::direction::up, v_scene_coord(1));
	ax2->add_child(arrow_to_a0a1x2);
	arrow_to_a0a1x2->set_anchor(point<2>(0,0));
	arrow_to_a0a1x2->set_anchor_style(arrow_node::anchor_style_type::tip);

	// split first column into a0 columns, each of which is 1x wide and a1 x's high (group 1)
	group1 = col1->split(dimension::y,a0);
	p_rect::set_children_display_style(group1,p_rect::display_style_type::center_expanded);
	pause(brief_pause);

	// move f_a0a1x2 into line
	auto after_ax2 = quad_expr->split(std::find(quad_expr->begin(),quad_expr->end(),p1));
	quad_expr->remove(ax2);

	render_action(make_move_action<shift>(
		after_ax2,0,f_a0a1x2->bounding_rect().width - ax2->own_bounding_rect().width,0));
	pause(brief_pause);
	render_action(make_move_action<shift>(f_a0a1x2,0,0,-f_a0a1x2->get_location().y));
	pause(brief_pause);
	quad_expr->splice(quad_expr->end(),f_a0a1x2);
	quad_expr->splice(quad_expr->end(),after_ax2);

	if (a0 > 1) {
		// shift group2a, group 2b, and group3 right
		// to make room for stacking of group1
		auto shift_2b_3 = make_move_action<shift>(rect_2b_3,0,(a0 - 1) * var_val * unit_size, 0);
		auto shift_2a = make_move_action<shift>(group2a,0,(a0 - 1) * var_val * unit_size, 0);
		auto shift_2a_2b_3 = std::make_shared<action>();
		shift_2a_2b_3->add_child(shift_2a);
		shift_2a_2b_3->add_child(shift_2b_3);
		render_action(shift_2a_2b_3);

		// stack group1 horizontally
		render_action(make_move_action<stack_action>(group1,0,dimension::x,unit_size));
	}

	/**** factor gcd -- a0x -- out of term 1 and term 2 in the quad_expr ****/
	// rewrite a0*a1x^2 as a0x*a1x
	auto a0x_term_1 = std::make_shared<text_node>(std::to_string(a0) + var_name);
	auto times_btw_a0x_a1x = std::make_shared<text_node>("*");
	auto a1x_term_1 = std::make_shared<text_node>(std::to_string(a1) + var_name);
	auto a0xa1x = std::make_shared<lineup_node>(dimension::x);
	a0xa1x->set_lineup(unique_sequence<std::shared_ptr<node>>(
			{a0x_term_1,times_btw_a0x_a1x,a1x_term_1}
		)
	);
	a0xa1x->set_location(x2->get_location().x,v_scene_coord(-2),0);
	quad_expr->add_child(a0xa1x);

	auto arrow_to_a0_term_1 = std::make_shared<arrow_node>(arrow_node::direction::up, v_scene_coord(1));
	a0_term_1->add_child(arrow_to_a0_term_1);
	arrow_to_a0_term_1->set_anchor(point<2>(0,0));
	arrow_to_a0_term_1->set_anchor_style(arrow_node::anchor_style_type::tip);

	auto after_term_1 = quad_expr->split(p1);
	int after_term_1_delta = a0xa1x->bounding_rect().width - after_term_1->get_location().x;
	quad_expr->erase(quad_expr->begin(),quad_expr->end());
	if (after_term_1_delta > 0) {
		render_action(make_move_action<shift>(after_term_1,0,after_term_1_delta,0));
	}
	render_action(make_move_action<shift>(a0xa1x,0,0,-a0xa1x->get_location().y));
	quad_expr->splice(quad_expr->end(),a0xa1x);
	quad_expr->splice(quad_expr->end(),after_term_1);


	// extract a0x from both a0xa1x and a0b1x



	// switcheroo


	// reinsert a0 into the lineup


	// add parentheses

	// group 1 and 2a under a common parent, preserving scene locations
	auto group_1_2a = std::make_shared<node>();
	v.scn->get_root()->add_child(group_1_2a);
	group_1_2a->set_location(group1->get_scene_location());
	p_rects->add_child(group_1_2a,true);
	group_1_2a->add_child(group1,true);
	group_1_2a->add_child(group2a,true);

	// group2 no longer has any children.  remove it from p_rects.
	p_rects->remove_child(group2);

	auto rect1 = p_rect::merge(group1,dimension::y);
	auto rect2a = p_rect::merge(group2a,dimension::y);

	flash_display_style(group_1_2a->get_children(),p_rect::display_style_type::center_expanded,.5,2.0,.5);

	// stack group_1_2a vertically
	render_action(make_move_action<stack_action>(group_1_2a,0,dimension::y));

	auto rect_1_2a = p_rect::merge(group_1_2a, dimension::x);
	flash_display_style({rect_1_2a},p_rect::display_style_type::center_factored,.5,2.0,.5);
	
	// join left and right
	render_action(make_move_action<shift>(rect_2b_3,0,-(group_2b_3->get_location().x - group_1_2a->bounding_rect().width + 1),0));

	auto rect_all = p_rect::merge(p_rects, dimension::y);
	flash_display_style({rect_all},p_rect::display_style_type::center_factored,.5,2.0,.5);

	return animation_;
}

void quad_factor_animator::flash_display_style(std::vector<std::shared_ptr<node>> p_rects, p_rect::display_style_type ds, double pause_before, double pause_during, double pause_after) {
	std::vector<p_rect::display_style_type> styles{};
	for (auto pr : p_rects) {
		styles.push_back(std::dynamic_pointer_cast<p_rect>(pr)->get_display_style());
	}
	pause(pause_before);
	for (auto pr : p_rects) {
		std::dynamic_pointer_cast<p_rect>(pr)->set_display_style(ds);	
	}
	pause(pause_during);
	for (int i = 0; i < p_rects.size(); ++i) {
		std::dynamic_pointer_cast<p_rect>(p_rects[i])->set_display_style(styles[i]);
	}
	pause(pause_after);
}




