#include "animation.h"
#include <chrono>
#include <thread>

using namespace liven;

template <class VIEWER>
void animation::present(VIEWER &v) {
	for (auto frame : frames) {
		v->show(frame);
		std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(1000 / fps)));
	}
}

void animation::set_animation_speed(double speed) {
	fps = speed;
}

int animation::get_animation_speed() {
	return fps;
}

void animation::set_animation_period(double period) {
	fps = 1 / period;
}

int animation::get_animation_period() {
	return 1 / fps;
}
