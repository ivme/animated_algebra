#ifndef QUAD_FACTOR_ANIMATOR_H
#define QUAD_FACTOR_ANIMATOR_H

#include "animator.h"
#include "p_rect.h"
#include <string>
#include <queue>

class quad_factor_animator : public animator {
public:
	quad_factor_animator(int a, int b, int c);
	virtual void set_up_scene() override;
	virtual std::shared_ptr<animation<wchar_t>> animate() override;
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
	std::shared_ptr<node> group1, group2, group2a, group2b, group3;
	std::string var_name;
	int var_val;

	std::queue<std::shared_ptr<action>> action_queue;
	void initialize_columns();
	std::set<int> get_split_points(std::shared_ptr<p_rect> rect_to_cut, unsigned int sub_rect_count) const;
};

#endif
