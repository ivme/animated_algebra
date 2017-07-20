#ifndef QUAD_FACTOR_ANIMATOR_H
#define QUAD_FACTOR_ANIMATOR_H

#include "p_rect.h"
#include "liven/animator.h"
#include "liven/text_node.h"
#include "liven/arrow_node.h"
#include "liven/lineup_node.h"
#include <string>
#include <queue>

class quad_factor_animator : public liven::animator {
public:
	quad_factor_animator(int a, int b, int c);
	virtual void set_up_scene() override;
	virtual liven::animation<liven::ascii_image> animate() override;
	// virtual void set_up_actions() override;
	// virtual void will_render_frame() override;
	// virtual void did_render_frame() override;

	static constexpr char const *default_var_name = "x";
	static constexpr int default_var_val = 2;

#ifndef PRIVACY_OFF
private:
#endif
	int a, b, c; // quadratic coeffs, ax^2 + bx + c
	std::shared_ptr<p_rect> col1, col2, col2a, col2b, col3;
	std::shared_ptr<liven::text_node> ax2_p_bx_p_c; // ax^2 + bx + c, one node
	std::shared_ptr<liven::text_node> ax2, p1, bx1, p2, cx0; // ax^2 + bx + c, 5 nodes
	std::shared_ptr<liven::text_node> a0b1x, a1b0x, p3; // ax^2 + a0b1x + a1b0x + c
	std::shared_ptr<liven::text_node> a0x_a1x_p_b1, b0_a1x_p_b1; // a0x(a1x + b1) + b0(a1x + b1)
	std::shared_ptr<liven::node> text_line, p_rects, group1, group2, group2a, group2b, group3;
	std::string var_name;
	int var_val;
	std::shared_ptr<liven::lineup_node> quad_expr;

	// move settings
	liven::move::frame_count_method_type fcm = liven::move::frame_count_method_type::fixed;
	int fixed_frame_count = 5;
	int node_speed = 2;
	int high_frame_limit = 15;
	int low_frame_limit = 3;

	void apply_move_settings(std::shared_ptr<liven::move> n);

	template <class MOVE_TYPE, typename... Args>
	std::shared_ptr<MOVE_TYPE> make_move_action(Args... args) {
		auto action_ = std::make_shared<MOVE_TYPE>(args...);
		apply_move_settings(action_);
		return action_;
	}

	// renders the children of parent with
	// a different display style, then reverts
	// to the original display styles
	// pause_before == length (seconds) of pause before changing display style
	// pause_during == length (seconds) of time for which children are displayed with the new style
	// pause_after == length (seconds) of pause after reverting to original display styles
	void flash_display_style(std::vector<std::shared_ptr<liven::node>> p_rects, p_rect::display_style_type ds, double pause_before, double pause_during, double pause_after);

	std::queue<std::shared_ptr<liven::action>> action_queue;
	void initialize_columns();
	std::set<int> get_split_points(std::shared_ptr<p_rect> rect_to_cut, unsigned int sub_rect_count) const;
	
	// draw an arrow with tip pointing toward n
	// label placed at tail of arrow
	void label(
		std::shared_ptr<liven::node> n,
		std::shared_ptr<liven::node> label,
		liven::arrow_node::direction dir = liven::arrow_node::direction::up,
		int arrow_length = 1 /*in pixels*/
	);
	
	// erase old_node_it.
	// slide the "tail lineup", the portion 
	// of the lineup that comes after the old_node_it,
	// horizontally to make room for new_node
	// return the tail lineup
	std::shared_ptr<liven::lineup_node> slide(
		std::shared_ptr<liven::node> old_node,
		std::shared_ptr<liven::node> new_node
	);

	// move new_node into place (at end of quad_expr)
	void place(std::shared_ptr<liven::node> n);

	// splice new_node and tail_lineup onto quad_expr
	void splice(
		std::shared_ptr<liven::lineup_node> new_node,
		std::shared_ptr<liven::lineup_node> tail_lineup
	);

	// slide, then place (no splicing at the end)
	// return a pointer to the tail lineup
	std::shared_ptr<liven::lineup_node> slide_place(
		std::shared_ptr<liven::node> old_node,
		std::shared_ptr<liven::lineup_node> new_node
	);

	// slide, place, then splice the new_node
	// and the tail_lineup onto quad_expr
	void substitute(
		std::shared_ptr<liven::node> old_node,
		std::shared_ptr<liven::lineup_node> new_node
	);

	// label old_node with new_node, then substitute
	// new_node for old_node
	void label_sub(
		std::shared_ptr<liven::node> old_node,
		std::shared_ptr<liven::lineup_node> new_node,
		double pause_after_label = 1.0,
		liven::arrow_node::direction dir = liven::arrow_node::direction::up,
		int arrow_length = 1 /*in pixels*/
	);

	// animate swap of nodes
	// assume nodes are neighbors along dimension::x
	void swap_neighbors(
		std::shared_ptr<liven::node> n,
		std::shared_ptr<liven::node> m
	);

	// factor_out
	// ecf0 : "enclosed common factor 0"
	// eof0 : "enclosed other fator 0"
	// pm : "plus or minus" (one but not the other)
	// ecf1 : "enclosed common factor 1"
	// e0f1 : "enclosed other factor 1"
	// animates factoring out a common factor as follows:
	// (cf)(of1) + (cf)(of2) = cf(of1 + of2)
	std::shared_ptr<liven::lineup_node> factor_out(
		std::shared_ptr<liven::lineup_node> ecf0,
		std::shared_ptr<liven::lineup_node> eof0,
		std::shared_ptr<liven::node> pm,
		std::shared_ptr<liven::lineup_node> ecf1,
		std::shared_ptr<liven::lineup_node> eof1
	);

	std::shared_ptr<liven::lineup_node> factor_out_right(
		std::shared_ptr<liven::node> eof0,
		std::shared_ptr<liven::lineup_node> ecf0,
		std::shared_ptr<liven::node> pm,
		std::shared_ptr<liven::node> ecf1,
		std::shared_ptr<liven::lineup_node> eof1
	);

	void stack_swap(std::shared_ptr<liven::node> sub_rects);
};

#endif
