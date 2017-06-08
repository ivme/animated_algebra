#include "p_rect.h"

 bool operator==(const length &lhs, const length &rhs) {return lhs.name == rhs.name && lhs.val == rhs.val;}

located<rect,2> p_rect::own_bounding_rect() const {
	return located<rect,2>(rect(width()+1,height()+1),get_location());
}

int p_rect::width() const {
	int out = 0;
	for (auto it = x_lengths.begin(); it != x_lengths.end(); ++it) {out += it->val;}
	return out;
}

int p_rect::height() const {
	int out = 0;
	for (auto it = y_lengths.begin(); it != y_lengths.end(); ++it) {out += it->val;}
	return out;
}