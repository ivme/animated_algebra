#include "quad_factor_animator.h"
#include "aa_renderer.h"
#include "p_rect.h"
#include "swap.h"
#include "liven/action.h"
#include "liven/stack_action.h"
#include "liven/arrow_node.h"
#include "algebra.h"
#include <typeinfo> //debug
#include <cassert> //debug

#define SHOW_RECTANGLES

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
using liven::space_children;
using liven::settle;

double brief_pause = 0.20;
double label_pause = 0.50;

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
	animation_.set_animation_speed(8);

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

	#ifdef SHOW_RECTANGLES
		p_rects->add_child(col1);
		pause(brief_pause);
		col1->set_display_style(p_rect::display_style_type::labels_and_lines);
		if (a > 1) {pause(brief_pause);}
	#endif

	quad_expr->push_back(p1);
	quad_expr->push_back(bx1);
	pause(brief_pause);

	#ifdef SHOW_RECTANGLES
		p_rects->add_child(col2);
		pause(brief_pause);
		col2->set_display_style(p_rect::display_style_type::labels_and_lines);
		if (b > 1) {pause(brief_pause);}
	#endif

	quad_expr->push_back(p2);
	quad_expr->push_back(cx0);
	pause(brief_pause);

	#ifdef SHOW_RECTANGLES
		p_rects->add_child(col3);
		pause(brief_pause);
		col3->set_display_style(p_rect::display_style_type::labels_and_lines);
		if (c > 1) {pause(brief_pause);}	
	#endif
}

void quad_factor_animator::apply_move_settings(std::shared_ptr<move> n) {
	n->use_frame_count_method(fcm);
	n->set_fixed_frame_count(fixed_frame_count);
	n->set_node_speed(node_speed);
	n->set_low_frame_limit(low_frame_limit);
	n->set_high_frame_limit(high_frame_limit);
}

animation<ascii_image> quad_factor_animator::animate() {
	animation_.set_animation_speed(3);

	// factor ax^2 + bx + c into (a0x + b0)(a1x + b1)
	std::tuple<bool,int,int,int,int> f = algebra::quad_factor(a,b,c);

	bool is_factorable = std::get<0>(f);
	if (!is_factorable) {throw std::runtime_error("not factorable");}
	int a0 = std::get<1>(f);
	int b0 = std::get<2>(f);
	int a1 = std::get<3>(f);
	int b1 = std::get<4>(f);

	/*
	Let s = a0b1 and q = a1b0.
	The complete factoring procedure can be written as follows:
	(0) ax^2 + bx + c
	(1) ax^2 + sx + tx + c
	(2) ax^2 + sx + tx + (b0)(b1)
	(3) ax^2 + sx + (a1x)(b0) + (b0)(b1)
	(4) ax^2 + sx + (b0)(a1x) + (b0)(b1)
	(5) ax^2 + sx + b0(a1x + b1)
	(6) ax^2 + (a0x)(b1) + b0(a1x + b1)
	(7) (a0x)(a1x) + (a0x)(b1) + b0(a1x + b1)
	(8) a0x(a1x + b1) + b0(a1x + b1)
	(9) (a0x + b0)(a1x + b1)

	Example:  6x^2 + 17x + 12 = (3x + 4)(2x + 3)
	(0) 6x^2 + 17x + 12
	(1) 6x^2 + 9x + 8x + 12
	(2) 6x^2 + 9x + 8x + (4)(3)
	(3) 6x^2 + 9x + (2x)(4) + (4)(3)
	(4) 6x^2 + 9x + (4)(2x) + (4)(3)
	(5) 6x^2 + 9x + 4(2x + 3)
	(6) 6x^2 + (3x)(3) + 4(2x + 3)
	(7) (3x)(2x) + (3x)(3) + 4(2x + 3)
	(8) 3x(2x + 3) + 4(2x + 3)
	(9) (3x + 4)(2x + 3)
	*/

	int unit_size = p_rect::unit_size;
	
	set_up_scene();

	// (1) ax^2 + sx + tx + c
	//            +-----+
	auto sx = std::make_shared<text_node>(algebra::term_to_string(a0 * b1, var_name));
	auto tx = std::make_shared<text_node>(algebra::term_to_string(a1 * b0, var_name));
	auto p3 = std::make_shared<text_node>(" + ");
	auto sx_p_tx = std::make_shared<lineup_node>(dimension::x);
	sx_p_tx->set_lineup({sx, p3, tx});

	label(bx1,sx_p_tx);
	pause(label_pause);

	#ifdef SHOW_RECTANGLES
		// split second column into two sub-columns
		// col 2a:  1x wide by a0*b1 1's high
		// col 2b:  1x wide by a1*b0 1's high
		// call the parent of these two sub-columns group2
		std::set<int> split_points = {a0 * b1};
		group2 = col2->split(dimension::y,split_points);
		col2a = std::dynamic_pointer_cast<p_rect>(group2->get_children()[0]);
		col2b = std::dynamic_pointer_cast<p_rect>(group2->get_children()[1]);
		p_rect::set_children_display_style(group2,p_rect::display_style_type::center_expanded);
		pause(brief_pause);
	#endif

	substitute(bx1,sx_p_tx);

	#ifdef SHOW_RECTANGLES
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
		pause(label_pause);
	#endif

	// (2) ax^2 + sx + tx + (b0)(b1)
	//                      +------+
	//                       term_4
	auto term_4 = std::make_shared<lineup_node>(dimension::x);
	auto lp_b0_term_4 = std::make_shared<text_node>("(");
	auto b0_term_4 = std::make_shared<text_node>(std::to_string(b0));
	auto rp_b0_term_4 = std::make_shared<text_node>(")");
	auto closed_b0_term_4 = std::make_shared<lineup_node>(dimension::x);
	closed_b0_term_4->set_lineup({lp_b0_term_4, b0_term_4, rp_b0_term_4});

	auto lp_b1_term_4 = std::make_shared<text_node>("(");
	auto b1_term_4 = std::make_shared<text_node>(std::to_string(b1));
	auto rp_b1_term_4 = std::make_shared<text_node>(")");
	auto closed_b1_term_4 = std::make_shared<lineup_node>(dimension::x);
	closed_b1_term_4->set_lineup({lp_b1_term_4,b1_term_4,rp_b1_term_4});

	term_4->set_lineup({closed_b0_term_4,closed_b1_term_4});
	label(cx0,term_4);
	pause(label_pause);

	#ifdef SHOW_RECTANGLES
		// split 3rd column into b0 p_rects, each of which is 1 wide and b1 1's high (group 3)
		// set parent of resulting collection of p_rects to group3
		group3 = col3->split(dimension::y,b0);
		p_rect::set_children_display_style(group3,p_rect::display_style_type::center_expanded);
		pause(label_pause);
	#endif

	// remove cx0 and move term_4 into line with the rest of quad_expr
	substitute(cx0,term_4);

	#ifdef SHOW_RECTANGLES
		if (b0 > 1) {
			// stack_horizontal group3
			render_action(make_move_action<stack_action>(group3,0,dimension::x,unit_size / 2,0));
		}
		// merge group3 into a single p_rect
		auto rect3 = p_rect::merge(group3, dimension::y);
		rect3->set_display_style(p_rect::display_style_type::labels_and_lines);
	#endif

	// (3) ax^2 + sx + (b0)(a1x) + (b0)(b1)
	//                 +-------+
	//                   term 3
	auto term_3 = std::make_shared<lineup_node>(dimension::x);

	auto lp_a1x_term_3 = std::make_shared<text_node>("(");
	auto a1x_term_3 = std::make_shared<text_node>(algebra::term_to_string(a1,var_name));
	auto rp_a1x_term_3 = std::make_shared<text_node>(")");
	auto closed_a1x_term_3 = std::make_shared<lineup_node>(dimension::x);
	closed_a1x_term_3->set_lineup({lp_a1x_term_3,a1x_term_3,rp_a1x_term_3});

	auto lp_b0_term_3 = std::make_shared<text_node>("(");
	auto b0_term_3 = std::make_shared<text_node>(std::to_string(b0));
	auto rp_b0_term_3 = std::make_shared<text_node>(")");
	auto closed_b0_term_3 = std::make_shared<lineup_node>(dimension::x);
	closed_b0_term_3->set_lineup({lp_b0_term_3,b0_term_3,rp_b0_term_3});

	term_3->set_lineup(unique_sequence<std::shared_ptr<node>>(
		{closed_b0_term_3,closed_a1x_term_3}));
	label(tx,term_3);
	pause(label_pause);

	#ifdef SHOW_RECTANGLES
		// split column 2b into a group of subcolumns
		// group 2b: b0 columns, each of which is 1x wide and a1 1's high
		group2b = col2b->split(dimension::y,b0);

		p_rect::set_children_display_style(group2b,p_rect::display_style_type::center_expanded);
		pause(brief_pause);
	#endif
	
	substitute(tx,term_3);

	#ifdef SHOW_RECTANGLES
		// widest it will be, in scene coordinates, is
		// width of a horizontally stacked subrect + unit_size * (# subrects - 1)
		int width_hs_subrect = a1 * var_val * unit_size;
		int widest = width_hs_subrect + unit_size * (b0 - 1);
		// extra width beyond original width
		int extra_width = widest - var_val*unit_size;

		// shift rect3 right to make space for stacking of group2b
		render_action(make_move_action<shift>(rect3,0,extra_width,0));

		stack_swap(group2b);

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
	#endif

	/*
	// (4) ax^2 + sx + (b0)(a1x) + (b0)(b1)
	//                 +-------+
	// swap a1x and b0 in term 3
	swap_neighbors(closed_a1x_term_3,closed_b0_term_3);
	#ifdef SHOW_RECTANGLES
	rect2b->swap_x_y();
	#endif
	pause(.5);
	*/

	// (5) ax^2 + sx + b0(a1x + b1)
	auto factored_terms_3_4 = factor_out(
		closed_b0_term_3,
		closed_a1x_term_3,
		p2,
		closed_b0_term_4,
		closed_b1_term_4);
	quad_expr->erase(std::find(quad_expr->begin(),quad_expr->end(),closed_b0_term_3),quad_expr->end(),false);
	auto a1x_b1_1 = std::dynamic_pointer_cast<lineup_node>(*(++factored_terms_3_4->begin()));
	quad_expr->splice(quad_expr->end(),factored_terms_3_4);

	#ifdef SHOW_RECTANGLES
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
	#endif

	// (6) ax^2 + (a0x)(b1) + b0(a1x + b1)
	//            +-------+
	//              term_2
	auto lp_a0x_term_2 = std::make_shared<text_node>("(");
	auto a0x_term_2 = std::make_shared<text_node>(algebra::term_to_string(a0,var_name));
	auto rp_a0x_term_2 = std::make_shared<text_node>(")");
	auto closed_a0_x_term_2 = std::make_shared<lineup_node>(dimension::x);
	closed_a0_x_term_2->set_lineup({lp_a0x_term_2,a0x_term_2,rp_a0x_term_2});
	
	auto lp_b1_term_2 = std::make_shared<text_node>("(");
	auto b1_term_2 = std::make_shared<text_node>(std::to_string(b1));
	auto rp_b1_term_2 = std::make_shared<text_node>(")");
	auto closed_b1_term_2 = std::make_shared<lineup_node>(dimension::x);
	closed_b1_term_2->set_lineup({lp_b1_term_2,b1_term_2,rp_b1_term_2});

	auto term_2 = std::make_shared<lineup_node>(dimension::x);
	term_2->set_lineup({closed_a0_x_term_2,closed_b1_term_2});
	label(sx,term_2);
	pause(label_pause);

	#ifdef SHOW_RECTANGLES
		// split column 2a into a group of subcolumns
		// group 2a: a0 columns, each of which is 1x wide and b1 1's high
		group2a = col2a->split(dimension::y,a0);
		p_rect::set_children_display_style(group2a,p_rect::display_style_type::center_expanded);
	#endif
	
	substitute(sx,term_2);

	#ifdef SHOW_RECTANGLES
		if (a0 > 1) {
			// shift rect2b
			render_action(make_move_action<shift>(rect_2b_3,0,(a0 - 1) * var_val * unit_size,0));

			// stack group2a horizontally
			render_action(make_move_action<stack_action>(group2a,0,dimension::x,unit_size));
		}
	#endif

	// (7) (a0x)(a1x) + (a0x)(b1) + b0(a1x + b1)
	//     +--------+
	//       term_1
	auto lp_a0x_term_1 = std::make_shared<text_node>("(");
	auto a0x_term_1 = std::make_shared<text_node>(algebra::term_to_string(a0,var_name));
	auto rp_a0x_term_1 = std::make_shared<text_node>(")");
	auto closed_a0x_term_1 = std::make_shared<lineup_node>(dimension::x);
	closed_a0x_term_1->set_lineup({lp_a0x_term_1,a0x_term_1,rp_a0x_term_1});

	auto lp_a1x_term_1 = std::make_shared<text_node>("(");
	auto a1x_term_1 = std::make_shared<text_node>(algebra::term_to_string(a1,var_name));
	auto rp_a1x_term_1 = std::make_shared<text_node>(")");
	auto closed_a1x_term_1 = std::make_shared<lineup_node>(dimension::x);
	closed_a1x_term_1->set_lineup({lp_a1x_term_1,a1x_term_1,rp_a1x_term_1});

	auto term_1 = std::make_shared<lineup_node>(dimension::x);
	term_1->set_lineup({closed_a0x_term_1,closed_a1x_term_1});
	label(ax2,term_1);
	pause(label_pause);

	#ifdef SHOW_RECTANGLES
		// split first column into a0 columns, each of which is 1x wide and a1 x's high (group 1)
		group1 = col1->split(dimension::y,a0);
		//p_rect::set_children_display_style(group1,p_rect::display_style_type::center_expanded);
		pause(brief_pause);
	#endif
	// move term_1 into line
	substitute(ax2,term_1);

	#ifdef SHOW_RECTANGLES
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
	#endif

	// (8) a0x(a1x + b1) + b0(a1x + b1)
	auto factored_terms_1_2 = factor_out(
		closed_a0x_term_1,
		closed_a1x_term_1,
		p1,
		closed_a0_x_term_2,
		closed_b1_term_2);

	auto a1x_b1_0 = std::dynamic_pointer_cast<lineup_node>(*(++factored_terms_1_2->begin()));	
	// slide factored_terms_3_4 toward factored_terms_1_2
	int p3_target_x = quad_expr->get_scene_location().x + factored_terms_1_2->bounding_rect().width;
	int delta = p3_target_x - p3->get_scene_location().x;
	auto slide_b0_term_3_and_a1x_b1_1 = std::make_shared<action>();
	slide_b0_term_3_and_a1x_b1_1->add_child(make_move_action<shift>(p3,0,delta,0));
	slide_b0_term_3_and_a1x_b1_1->add_child(make_move_action<shift>(b0_term_3,0,delta,0));
	slide_b0_term_3_and_a1x_b1_1->add_child(make_move_action<shift>(a1x_b1_1,0,delta,0));
	quad_expr->erase(quad_expr->begin(),std::find(quad_expr->begin(),quad_expr->end(),p3),false);
	quad_expr->splice(quad_expr->begin(),factored_terms_1_2,false);
	render_action(slide_b0_term_3_and_a1x_b1_1);

	#ifdef SHOW_RECTANGLES
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
		//flash_display_style({rect_1_2a},p_rect::display_style_type::center_factored,.5,2.0,.5);
	#endif

	// (9) (a0x + b0)(a1x + b1)
	auto full_factored_form = factor_out_right(
		a0x_term_1,
		a1x_b1_0,
		p3,
		b0_term_3,
		a1x_b1_1
	);
	quad_expr->erase(quad_expr->begin(),quad_expr->end());
	quad_expr->splice(quad_expr->begin(),full_factored_form);

	#ifdef SHOW_RECTANGLES
		
		// join left and right
		render_action(make_move_action<shift>(rect_2b_3,0,-(group_2b_3->get_location().x - group_1_2a->bounding_rect().width + 1),0));

		auto rect_all = p_rect::merge(p_rects, dimension::y);
		flash_display_style({rect_all},p_rect::display_style_type::center_factored,.5,2.0,.5);
	#endif

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

void quad_factor_animator::label(
	std::shared_ptr<node> n,
	std::shared_ptr<node> label,
	arrow_node::direction dir,
	int arrow_length)
{
	quad_expr->add_child(label);
	label->set_location(n->get_location().x,v_scene_coord(-arrow_length-1),0);

	// make arrow
	auto arrow_ = std::make_shared<arrow_node>(dir, v_scene_coord(arrow_length));
	n->add_child(arrow_);
	arrow_->set_anchor(point<2>(0,0));
	arrow_->set_anchor_style(arrow_node::anchor_style_type::tip);
}

std::shared_ptr<lineup_node> quad_factor_animator::slide(
	std::shared_ptr<node> old_node,
	std::shared_ptr<node> new_node)
{
	auto old_node_it = std::find(quad_expr->begin(),quad_expr->end(),old_node);
	if (old_node_it == quad_expr->end()) {throw std::runtime_error("old_node not found in quad_expr");}
	auto first_tail_it = old_node_it;
	++first_tail_it;
	auto tail_lineup = quad_expr->split(first_tail_it);
	quad_expr->erase(old_node_it,false);
	int tail_lineup_delta = new_node->bounding_rect().width - old_node->bounding_rect().width;
	render_action(make_move_action<shift>(tail_lineup,0,tail_lineup_delta,0));
	return tail_lineup;
}

void quad_factor_animator::place(std::shared_ptr<node> n) {
	render_action(make_move_action<shift>(n,0,0,-n->get_location().y));
}

void quad_factor_animator::splice(
	std::shared_ptr<lineup_node> new_node,
	std::shared_ptr<lineup_node> tail_lineup)
{
	quad_expr->splice(quad_expr->end(),new_node);
	quad_expr->splice(quad_expr->end(),tail_lineup);
}

std::shared_ptr<lineup_node> quad_factor_animator::slide_place(
	std::shared_ptr<node> old_node,
	std::shared_ptr<lineup_node> new_node)
{
	auto out = slide(old_node,new_node);
	place(new_node);
	return out;
}

void quad_factor_animator::substitute(
	std::shared_ptr<node> old_node,
	std::shared_ptr<lineup_node> new_node)
{
	auto tail_lineup = slide_place(old_node,new_node);
	splice(new_node,tail_lineup);
}

void quad_factor_animator::label_sub(
	std::shared_ptr<node> old_node,
	std::shared_ptr<lineup_node> new_node,
	double pause_after_label,
	arrow_node::direction dir,
	int arrow_length)
{
	label(old_node,new_node,dir,arrow_length);
	pause(pause_after_label);
	substitute(old_node,new_node);
}

void quad_factor_animator::swap_neighbors(
		std::shared_ptr<liven::node> n,
		std::shared_ptr<liven::node> m)
{
	auto start_y = n->get_scene_location().y;
	if (start_y != m->get_scene_location().y) {
		throw std::runtime_error("neighbors to be swapped have mismatched y-coordinates");
	}
	auto split_up_down = std::make_shared<action>();
	split_up_down->add_child(make_move_action<shift>(n,0,0,v_scene_coord(-1)));
	split_up_down->add_child(make_move_action<shift>(m,0,0,v_scene_coord(1)));
	render_action(split_up_down);
	auto swap_left_right = std::make_shared<action>();
	int left_delta = -n->bounding_rect().width;
	int right_delta = m->bounding_rect().width;
	swap_left_right->add_child(make_move_action<shift>(n,0,right_delta,0));
	swap_left_right->add_child(make_move_action<shift>(m,0,left_delta,0));
	render_action(swap_left_right);
	auto converge_up_down = std::make_shared<action>();
	converge_up_down->add_child(make_move_action<shift>(n,0,0,-n->get_location().y));
	converge_up_down->add_child(make_move_action<shift>(m,0,0,-m->get_location().y));
	render_action(converge_up_down);
}

std::shared_ptr<lineup_node> quad_factor_animator::factor_out(
		std::shared_ptr<liven::lineup_node> ecf0,
		std::shared_ptr<liven::lineup_node> eof0,
		std::shared_ptr<liven::node> pm,
		std::shared_ptr<liven::lineup_node> ecf1,
		std::shared_ptr<liven::lineup_node> eof1)
{
	// (cf) (of0)  +  (cf) (of1)
	//  ^     ^    ^   ^     ^
	// efc0 eof0  pm  ecf1 e0f1
	//
	// hide parens
	//  cf (of0) +  cf (of1)
	// 
	// double drop:
	//     (of0) +     (of1)
	//  cf          cf
	//
	// hide parens
	//     (of0  +      of1)
	//  cf          cf
	//
	// switcheroo
	//   (of0 + of1)
	// cf
	//
	// place cf
	// cf(of0 + of1)

	auto it = ecf0->begin();
	auto lp_cf0 = *it;
	auto cf0 = *(++it);
	auto rp_cf0 = *(++it);

	it = eof0->begin();
	auto lp_of0 = *it;
	auto of0 = *(++it);
	auto rp_of0 = *(++it);

	it = ecf1->begin();
	auto lp_cf1 = *it;
	auto cf1 = *(++it);
	auto rp_cf1 = *(++it);

	it = eof1->begin();
	auto lp_of1 = *it;
	auto of1 = *(++it);
	auto rp_of1 = *(++it);

	// hide parens
	lp_cf0->is_visible = false;
	rp_cf0->is_visible = false;
	lp_cf1->is_visible = false;
	rp_cf1->is_visible = false;

	// double drop
	auto double_drop = std::make_shared<action>();
	double_drop->add_child(make_move_action<shift>(cf0,0,0,v_scene_coord(-1)));
	double_drop->add_child(make_move_action<shift>(cf1,0,0,v_scene_coord(-1)));
	render_action(double_drop);

	// remove parens
	rp_of0->is_visible = false;
	lp_of1->is_visible = false;

	// switcheroo
	// targets
	int cf_target_x = cf0->get_scene_location().x;
	int eof0_target_x = cf_target_x + cf0->bounding_rect().width;
	int pm_target_x = eof0_target_x + lp_of0->bounding_rect().width + of0->bounding_rect().width;
	int eof1_target_x = pm_target_x + pm->bounding_rect().width - lp_of1->bounding_rect().width;

	// deltas
	int cf0_delta = cf_target_x - cf0->get_scene_location().x;
	int cf1_delta = cf_target_x - cf1->get_scene_location().x;
	int eof0_delta = eof0_target_x - eof0->get_scene_location().x;
	int pm_delta = pm_target_x - pm->get_scene_location().x;
	int eof1_delta = eof1_target_x - eof1->get_scene_location().x;
	
	auto switcheroo = std::make_shared<action>();
	switcheroo->add_child(make_move_action<shift>(cf0,0,cf0_delta,0));
	switcheroo->add_child(make_move_action<shift>(cf1,0,cf1_delta,0));
	switcheroo->add_child(make_move_action<shift>(eof0,0,eof0_delta,0));
	switcheroo->add_child(make_move_action<shift>(pm,0,pm_delta,0));
	switcheroo->add_child(make_move_action<shift>(eof1,0,eof1_delta,0));
	render_action(switcheroo);

	cf1->is_visible = false;
	place(cf0);
	auto right_factor = std::make_shared<lineup_node>(dimension::x);
	right_factor->set_lineup({lp_of0,of0,pm,of1,rp_of1});

	auto out = std::make_shared<lineup_node>(dimension::x);
	out->set_lineup({cf0,right_factor});
	return out;
}

std::shared_ptr<liven::lineup_node> quad_factor_animator::factor_out_right(
		std::shared_ptr<liven::node> of0,
		std::shared_ptr<liven::lineup_node> ecf0,
		std::shared_ptr<liven::node> pm,
		std::shared_ptr<liven::node> of1,
		std::shared_ptr<liven::lineup_node> ecf1)
{
	// do NOT remove parens on common factor

	// of0(cf0) + of1(cf1)
	// 
	// of0      + of1     
	//    (cf0)      (cf1)
	//
	// (of0     + of1)
	//      (cf0)        (cf1)
	//
	// (of0 + of1)
	//            (cf1)
	//
	// (of0 + of1)(cf1)

	// double drop
	auto double_drop = std::make_shared<action>();
	double_drop->add_child(make_move_action<shift>(ecf0,0,0,v_scene_coord(-1)));
	double_drop->add_child(make_move_action<shift>(ecf1,0,0,v_scene_coord(-1)));
	render_action(double_drop);

	// add parens
	auto lp_of0 = std::make_shared<text_node>("(");
	auto left_enclosed_of0 = std::make_shared<lineup_node>(dimension::x);
	left_enclosed_of0->set_location(of0->get_location());
	left_enclosed_of0->set_lineup({lp_of0,of0});
	quad_expr->add_child(left_enclosed_of0);
	quad_expr->remove_child(of0);

	auto rp_of1 = std::make_shared<text_node>(")");
	auto right_enclosed_of1 = std::make_shared<lineup_node>(dimension::x);
	right_enclosed_of1->set_location(of1->get_location());
	right_enclosed_of1->set_lineup({of1,rp_of1});
	quad_expr->add_child(right_enclosed_of1);
	quad_expr->remove_child(of1);
	
	// switcheroo
	// targets
	int pm_target_x = left_enclosed_of0->get_scene_location().x + left_enclosed_of0->bounding_rect().width;
	int right_enclosed_of1_target_x = pm_target_x + pm->bounding_rect().width;
	int cf_target_x = right_enclosed_of1_target_x + right_enclosed_of1->bounding_rect().width;
	
	// deltas
	int ecf0_delta = cf_target_x - ecf0->get_scene_location().x;
	int ecf1_delta = cf_target_x - ecf1->get_scene_location().x;
	int pm_delta = pm_target_x - pm->get_scene_location().x;
	int right_enclosed_of1_delta = right_enclosed_of1_target_x - right_enclosed_of1->get_scene_location().x;

	auto switcheroo = std::make_shared<action>();
	switcheroo->add_child(make_move_action<shift>(ecf0,0,ecf0_delta,0));
	switcheroo->add_child(make_move_action<shift>(ecf1,0,ecf1_delta,0));
	switcheroo->add_child(make_move_action<shift>(pm,0,pm_delta,0));
	switcheroo->add_child(make_move_action<shift>(right_enclosed_of1,0,right_enclosed_of1_delta,0));
	render_action(switcheroo);

	ecf1->set_parent(std::weak_ptr<node>());

	place(ecf0);
	auto out = std::make_shared<lineup_node>(dimension::x);
	out->set_lineup({left_enclosed_of0,pm,right_enclosed_of1,ecf0});

	return out;
}

void quad_factor_animator::stack_swap(std::shared_ptr<node> sub_rects) {
	int n = sub_rects->get_children().size();
	int grand_initial_spacing = 8;
	int frames_for_grand_spacing = 1;
	int sub_initial_spacing = 0;
	int node_speed = 1;
	int spacing_before_swap = 2*node_speed;
	int frames_to_settle = 5;
	
	// space the children of sub_rects
	auto grand_space_children = make_move_action<space_children>(sub_rects,0,0,grand_initial_spacing);
	//grand_space_children->set_fixed_frame_count(frames_for_grand_spacing);
	render_action(grand_space_children);
	pause(0.5);
	p_rect::set_children_display_style(sub_rects,p_rect::display_style_type::labels_and_lines);

	// stack each sub_rect horizontally
	auto stack_sub_rects = std::make_shared<action>();
	auto sub_rects_children = sub_rects->get_children();
	for (auto child_node : sub_rects_children) {
		auto sub_rect = std::dynamic_pointer_cast<p_rect>(child_node);
		auto sub_rect_node = sub_rect->split(dimension::y,sub_rect->get_lengths(dimension::y).size());
		auto stack_ = make_move_action<stack_action>(sub_rect_node,0,dimension::x,sub_initial_spacing,0);
		stack_sub_rects->add_child(stack_);
	}
	pause(.5);
	render_action(stack_sub_rects);
	pause(0.5);
	// merge the stacked p_rect groups into single p_rects
	sub_rects_children = sub_rects->get_children();
	for (auto sub_rect_node : sub_rects_children) {
		auto merged_rect = p_rect::merge(sub_rect_node,dimension::y);
	}

	// all remaining actions
	auto remaining_actions = std::make_shared<action>();
	for (int i = 0; i < n; ++i) {
		auto sub_rect = std::dynamic_pointer_cast<p_rect>(sub_rects->get_children()[i]);
		int delta = p_rect::unit_size * i;
		int frames_before_swap = (delta + p_rect::unit_size) / node_speed;
		
		auto shift_swap_settle = std::make_shared<action>();
		
		auto shift_ = std::make_shared<shift>(sub_rect,0,delta,0,move::frame_count_method_type::speed);
		shift_->set_node_speed(node_speed);
		if (i != 0) {shift_swap_settle->add_child(shift_);}

		auto swap_ = std::make_shared<swap>(sub_rect,frames_before_swap);
		shift_swap_settle->add_child(swap_);

		auto settle_ = std::make_shared<shift>(sub_rect,frames_before_swap + 1,0,-sub_rect->get_location().y);
		settle_->set_fixed_frame_count(frames_to_settle);
		shift_swap_settle->add_child(settle_);

		remaining_actions->add_child(shift_swap_settle);
	}
	render_action(remaining_actions);
}
