#ifndef P_RECT_H
#define P_RECT_H

#include <vector>
#include <string>
#include "renderer.h"

struct length {
public:
	length(std::string name_, unsigned int val_) : name(name_), val(val_) {}
	std::string name = "x";
	unsigned int val = 0;
};

bool operator==(const length &lhs, const length &rhs);

enum class dimension : int {x = 0, y = 1};

// "partitioned rectangle"
class p_rect : public node {
public:
	p_rect(std::vector<length> x_lengths_, std::vector<length> y_lengths_) : x_lengths(x_lengths_), y_lengths(y_lengths_) {}
	virtual located<rect,2> own_bounding_rect() const override;

	// split along specified dimension, x or y
	// split(x,{1,3}) splits the rectangle along the first vertical partition and the third vertical partition
	// return a node whose children are the result of the split
	// the children are indexed starting with closest to lower left at 0
	std::shared_ptr<node> split(dimension dim,std::set<int> split_points);

	// unit_size must be an integer multiple of both of the following
	// ascii_renderer::scene_x_coordinates_per_pixel
	// ascii_renderer::scene_y_coordinates_per_pixel
	static constexpr int unit_size = 4;
	int width() const;  // width in scene coordinates
	int height() const;  // height in scene coordinates
	int width_in_units() const;
	int height_in_units() const;
	std::vector<length> x_lengths;
	std::vector<length> y_lengths;

	virtual bool is_renderable() const override {return true;}
	virtual std::shared_ptr<ascii_image> render(renderer<ascii_image> &r) const override {return r.render(*this);};
	virtual std::shared_ptr<ascii_image> render(ascii_renderer &r) const override {return r.render(*this);}
};

#endif
