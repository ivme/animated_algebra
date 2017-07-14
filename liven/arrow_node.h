#ifndef ARROW_NODE_H
#define ARROW_NODE_H

#include "node.h"

namespace liven {

class arrow_node : public liven::node {
public:
	enum class direction : int {right = 0, left = 1, up = 2, down = 3};
	enum class anchor_style_type : int {tail = 0, tip = 1};

	arrow_node(direction d_, int length_) : node(this), d(d_), length(length_) {}
	arrow_node() : arrow_node(direction::right,1) {}

	void set_direction(direction d_) {d = d_;}
	direction get_direction() const {return d;}

	// length is measured in scene coordinates
	void set_length(int length_) {length = length_;}
	int get_length() const {return length;}

	point<2> get_anchor();
	void set_anchor(point<2> new_anchor);

	void anchor_tail();  // sets anchor point to tail of arrow
	void anchor_tip();  // sets anchor point to tip of arrow
	anchor_style_type get_anchor_style();
	void set_anchor_style(anchor_style_type style);

	// horizontal arrows are 2 scene coordinates high
	// veritcal arrows are 1 scene coordinate wide
	// (temporary fix to ensure compatibility with ascii rendering)
	static constexpr int h_thick = 2;
	static constexpr int v_thick = 1;

	virtual liven::located<rect,2> own_bounding_rect() const override;

private:
	direction d;
	int length;
	anchor_style_type anchor_style;

	bool anchor_is_at_positive(anchor_style_type) const; // anchor is at the positive end -- top or right
	bool direction_is_horizontal() const;
	point<2> get_offset(anchor_style_type) const; // get anchor's offset relative to location
};

}; // liven

#endif