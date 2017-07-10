#ifndef QUAD_FACTOR_ANIMATOR_H
#define QUAD_FACTOR_ANIMATOR_H

#include "animator.h"
#include "p_rect.h"
#include "text_node.h"
#include <string>
#include <queue>

class quad_factor_animator : public liven::animator {
public:
	quad_factor_animator(int a, int b, int c);
	virtual void set_up_scene() override;
	virtual std::shared_ptr<liven::animation<wchar_t>> animate() override;
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
	std::shared_ptr<liven::node> p_rects, group1, group2, group2a, group2b, group3;
	std::string var_name;
	int var_val;
	std::shared_ptr<liven::text_node> quad_expr;

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
};

#endif
