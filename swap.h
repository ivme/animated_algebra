#ifndef SWAP_H
#define SWAP_H

#include "liven/action.h"
#include "p_rect.h"

class swap : public liven::action {
public:
	swap(std::shared_ptr<p_rect> pr, int start_frame_) : action(pr,start_frame_) {}

	virtual bool own_act() override {
		std::dynamic_pointer_cast<p_rect>(n)->swap_x_y();
		return false;
	}
};

#endif