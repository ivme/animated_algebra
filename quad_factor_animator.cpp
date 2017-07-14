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

double brief_pause = .2;

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

	auto b0b1 = std::make_shared<text_node>(
		algebra::product_to_string(std::to_string(b0),std::to_string(b1))
		);
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

	// rewrite a0b1x in long factored form, below a0b1x, with arrow
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

	// split column 2b into a group of subcolumns
	// group 2b: a1 columns, each of which is 1x wide and b0 1's high
	group2b = col2b->split(dimension::y,a1);

	if (a1 > 1) {
		// shift rect3 right to make space for stacking of group2b
		render_action(make_move_action<shift>(rect3,0, (a1 - 1) * var_val * unit_size,0));

		// stack group2b horizontally
		render_action(make_move_action<stack_action>(group2b,0,dimension::x,unit_size / 2,0));
	}

	// merge group2b into a single p_rect
	auto rect2b = p_rect::merge(group2b, dimension::y);

	// if necessary, shift rect3 to make space
	// for swapping group2b's x's and y's
	int height_2b = rect2b->height();
	int width_2b = rect2b->width();
	if (height_2b > width_2b) {
		render_action(make_move_action<shift>(rect3,0,height_2b - width_2b,0));
	}

	// pause the animation for half a second
	// to clarify the swap visually 
	pause(.5);
	// swap x lengths and y lengths for group2b
	rect2b->swap_x_y();
	pause(.5);
	
	// group 2b and 3 under a common parent, preserving scene locations
	auto group_2b_3 = std::make_shared<node>();
	v.scn->get_root()->add_child(group_2b_3); // must be added to the scene for preserve_scene_location to work
	group_2b_3->set_location(rect2b->get_scene_location());
	p_rects->add_child(group_2b_3,true);
	group_2b_3->add_child(rect2b,true);
	group_2b_3->add_child(rect3,true);

	// group 1 and 2a under a common parent, preserving scene locations
	auto group_1_2a = std::make_shared<node>();
	v.scn->get_root()->add_child(group_1_2a);
	group_1_2a->set_location(group1->get_scene_location());
	p_rects->add_child(group_1_2a,true);
	group_1_2a->add_child(group1,true);
	group_1_2a->add_child(group2a,true);

	// group2 no longer has any children.  remove it from p_rects.
	p_rects->remove_child(group2);

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

	auto rect1 = p_rect::merge(group1,dimension::y);
	auto rect2a = p_rect::merge(group2a,dimension::y);

	flash_display_style(group_1_2a->get_children(),p_rect::display_style_type::center_expanded,.5,2.0,.5);

	// stack group_1_2a vertically
	render_action(make_move_action<stack_action>(group_1_2a,0,dimension::y));

	auto rect_1_2a = p_rect::merge(group_1_2a, dimension::x);
	flash_display_style({rect_1_2a},p_rect::display_style_type::center_factored,.5,2.0,.5);
	
	flash_display_style(group_2b_3->get_children(),p_rect::display_style_type::center_expanded,.5,2.0,.5);
	// stack group_2b_3 vertically
	render_action(make_move_action<stack_action>(group_2b_3,0,dimension::y));
	auto rect_2b_3 = p_rect::merge(group_2b_3, dimension::x);
	flash_display_style({rect_2b_3},p_rect::display_style_type::center_factored,.5,2.0,.5);
	
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




