#include "p_rect.h"

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