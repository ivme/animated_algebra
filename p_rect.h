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

// "partitioned rectangle"
class p_rect : public node {
public:
	p_rect(std::vector<length> x_lengths_, std::vector<length> y_lengths_) : x_lengths(x_lengths_), y_lengths(y_lengths_) {}
	virtual std::shared_ptr<ascii_image> render(renderer<ascii_image> &r) const override {return r.render(*this);};
	virtual std::shared_ptr<ascii_image> render(ascii_renderer &r) const override {return r.render(*this);}

	int width() const;
	int height() const;
	std::vector<length> x_lengths;
	std::vector<length> y_lengths;
};

#endif