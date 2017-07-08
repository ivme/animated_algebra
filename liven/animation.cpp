#include "animation.h"

using namespace liven;

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
