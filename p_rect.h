#ifndef P_RECT_H
#define P_RECT_H

#include <vector>
#include <string>
#include <map>
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
class p_rect : public liven::node {
public:
	enum display_style_type : int {empty = 0, labels_and_lines = 1, center_factored = 2, center_expanded = 3};

	p_rect(std::vector<length> x_lengths_, std::vector<length> y_lengths_) :
			node(this), x_lengths(x_lengths_), y_lengths(y_lengths_), display_style(labels_and_lines) {}
	virtual liven::rect own_bounding_rect() const override;

	// split along specified dimension, x or y
	// split(x,{1,3}) splits the rectangle along the first vertical partition and the third vertical partition
	// return a node whose children are the result of the split
	// the children are indexed starting with closest to lower left at 0
	std::shared_ptr<liven::node> split(liven::dimension dim,std::set<int> split_points);
	std::shared_ptr<liven::node> split(liven::dimension dim, int sub_rect_count);

	// merge the children of parent into a single p_rect
	// replace parent in the node tree with the merged p_rect
	// return a pointer to the merged p_rect
	// dim == x => merge along horizontal edges
	static std::shared_ptr<p_rect> merge(std::shared_ptr<liven::node> parent, liven::dimension dim); 

	// unit_size measures the size of a unit length in scene coordinates
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
	std::vector<length> x_lengths;
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
	static void set_children_display_style(std::shared_ptr<liven::node> n, display_style_type ds); // all children of n must be p_rects

	std::map<length,int> get_length_frequency_map(liven::dimension dim) const;
	std::map<std::string,int> get_var_coeff_map(liven::dimension dim) const;

	liven::grid_node get_grid_node() const;

#ifndef PRIVACY_OFF
private:
#endif
	display_style_type display_style;
	std::set<int> get_split_points(liven::dimension dim, unsigned int sub_rect_count) const;
};

#endif
