#include "animator.h"
#include "action.h"
#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <iostream> // debug

using namespace liven;

animator::animator() : v(), root_action(std::make_shared<action>()),
						animation_(animation<image_type>()) {}
animator::~animator() = default;

void animator::render_action() {
	render_action(root_action);
}

void animator::render_action(std::shared_ptr<action> action_) {
	remaining_action = true;

	while (remaining_action) {
		remaining_action = false;
		if (action_) {
			if (action_->act(frame_number)) {remaining_action = true;}
		}
		will_render_frame();
		animation_.append_frame(v.render());
		did_render_frame();
		++frame_number;
	}
}

void animator::pause(int frame_count) {
	for (int i = 0; i < frame_count; ++i) {snapshot();}
}

void animator::pause(double seconds) {
	pause((int) (animation_.get_animation_speed() * seconds));
}

void animator::snapshot() {
	animation_.append_frame(v.render());
}

animation<image_type> animator::animate() {
	return animation_;
}

void animator::set_root_action(std::shared_ptr<action> r) {
	root_action = r;
}
