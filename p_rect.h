#ifndef P_RECT_H
#define P_RECT_H

#include <vector>
#include <string>
#include <map>
#include <set>
#include "liven/graphics.h"
#include "liven/ascii_renderer.h"
#include "liven/action.h"
#include "liven/grid_node.h"

struct length {
public:
	length() : name("1"), val(1) {}
	length(std::string name_, int val_) : name(name_), val(val_) {}
	std::string name = "x";
	int val = 0;
};

bool operator==(const length &lhs, const length &rhs);
bool operator< (const length &lhs, const length &rhs);

// "partitioned rectangle"
class p_rect {
public:
	enum display_style_type : int {empty = 0, labels_and_lines = 1, center_factored = 2, center_expanded = 3};

	p_rect(std::vector<length> x_lengths_, std::vector<length> y_lengths_) :
			x_lengths(x_lengths_), y_lengths(y_lengths_), display_style(labels_and_lines) {}
	virtual liven::located<liven::rect,2> own_bounding_rect(std::shared_ptr<liven::node> n) const;

	struct split_response {
		std::shared_ptr<liven::node> parent;
		// a map from the child nodes produced by a split to the 
		// wrapped p_rects to which those children point
		std::map<std::shared_ptr<liven::node>, std::shared_ptr<p_rect>> children_wrapped;
	};

	// split along specified dimension, x or y
	// split(x,{1,3}) splits the rectangle along the first vertical partition and the third vertical partition
	// set parent of split_response::parent to be n's parent
	split_response split(std::shared_ptr<liven::node> &n, liven::dimension dim, std::set<int> split_points);
	split_response split(std::shared_ptr<liven::node> &n, liven::dimension dim, int sub_rect_count);

	struct merge_response {
		std::shared_ptr<liven::node> n;
		std::shared_ptr<p_rect> pr;
	};
	// merge some nodes (that wrap p_rects) into a single node
	// along the specified dimension
	// set the parent of the resulting node to parent
	static merge_response merge(
		std::shared_ptr<liven::node> parent,
		std::map<std::shared_ptr<liven::node>,std::shared_ptr<p_rect>> children_wrapped,
		liven::dimension dim);

	// unit_size measures the size of a unit in scene coordinates
	// unit_size must be an integer multiple of both of the following
	// ascii_renderer::scene_x_coordinates_per_pixel
	// ascii_renderer::scene_y_coordinates_per_pixel
	static constexpr int unit_size = 4;
	int width() const;  // width in scene coordinates
	int height() const;  // height in scene coordinates
	int width_in_units() const;
	int height_in_units() const;

	const std::vector<length> &get_lengths(liven::dimension dim) const;
	void swap_x_y();
	std::vector<length> x_lengths;  // length vals are measured in units, not scene coordinates
	std::vector<length> y_lengths;

	bool show_interior_lines = true;
	bool show_sub_rect_labels = true;
	bool show_center_expanded = false; // display an algebraic representation of the expanded form of this p_rect in the center of the p_rect
	bool show_center_factored = false; // display an algebraic representation of the factored form of this p_rect in the center of the p_rect 
	void add_left_label(); // labels vertical axis on left
	void add_bottom_label(); // labels whole p_rect in center of rect
	std::string x_label_text() const;
	std::string y_label_text() const;
	std::string get_factored_string() const;
	std::string get_expanded_string() const;
	std::string label_text(liven::dimension dim) const;
	void set_display_style(display_style_type ds);
	display_style_type get_display_style();

	std::map<length,int> get_length_frequency_map(liven::dimension dim) const;
	std::map<std::string,int> get_var_coeff_map(liven::dimension dim) const;

	liven::grid_node get_grid() const;

#ifndef PRIVACY_OFF
private:
#endif
	display_style_type display_style;
	std::set<int> get_split_points(liven::dimension dim, unsigned int sub_rect_count) const;
};

/*******            p_rect actions            *********/
/*
// does nothing for pause_before frames
// changes the display style, then does nothing for pause_during frames
// reverts to the original display style, then does nothing for pause_after frames
class flash_display_style : public action {
	flash_display_style(std::shared_ptr<p_rect> pr_, p_rect::display_style_type ds_,
						size_t pause_before_ = 1, size_t pause_during_ = 1, size_t pause_after_ = 1) : 
							pr(pr_), ds(ds_), pause_during(pause_during_), pause_after(pause_after_),
							change_frame(pause_before_), revert_frame(pause_before_ + pause_during_), 
							end_frame(pause_before_ + pause_during_ + pause_after_)
						{
							if (pr) {original_ds = pr->get_display_style();}
							else {throw std::runtime_error("flash_display_style called with null_ptr argument");}
						}
	virtual bool own_act() override;

#ifndef PRIVACY_OFF
private:
#endif
	std::shared_ptr<p_rect> pr;
	p_rect::display_style_type ds, original_ds;
	size_t pause_during, pause_after;
	size_t change_frame, revert_frame, end_frame;
};

// renders the children of parent with
// a different display style, then reverts
// to the original display styles
// pause_before == length (seconds) of pause before changing display style
// pause_during == length (seconds) of time for which children are displayed with the new style
// pause_after == length (seconds) of pause after reverting to original display styles
class flash_children_display_style : public action {
public:
	flash_children_display_style(std::shared_ptr<node> parent, double pause_before, double pause_during, double pause_after);
	virtual bool own_act() override;
};
*/
#endif
