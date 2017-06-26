#include "animator.h"
#include "algebra.h"
#include "action.h"
#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <iostream> // debug

animator::animator() : v(), r(), root_action(std::make_shared<action>()),
						animation_(std::make_shared<animation<wchar_t>>()) {}
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
		animation_->append_frame(v.render(r));
		did_render_frame();
		++frame_number;
	}
}

void animator::pause(int frame_count) {
	for (int i = 0; i < frame_count; ++i) {snapshot();}
}

void animator::pause(double seconds) {
	if (animation_) {
		pause((int) (animation_->get_animation_speed() * seconds));
	}
}

void animator::snapshot() {
	animation_->append_frame(v.render(r));
}

std::shared_ptr<animation<wchar_t>> animator::animate() {
	return animation_;
}


/*
std::shared_ptr<animation<wchar_t>> animator::animate() {
	auto out = std::make_shared<animation<wchar_t>>();
	out->set_animation_speed(4);

	frame_number = 0;
	remaining_action = true;

	while (remaining_action) {
		remaining_action = false;
		if (root_action) {
			if (root_action->act(frame_number)) {remaining_action = true;}
		}
		will_render_frame();
		out->append_frame(v.render(r));
		did_render_frame();
		++frame_number;
	}
	return out;
}
*/

void animator::set_root_action(std::shared_ptr<action> r) {
	root_action = r;
}
