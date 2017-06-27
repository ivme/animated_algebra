#include "animation.h"

template <class PIXEL_TYPE>
void animation<PIXEL_TYPE>::set_animation_speed(double speed) {
	fps = speed;
}

template <class PIXEL_TYPE>
int animation<PIXEL_TYPE>::get_animation_speed() {
	return fps;
}

template <class PIXEL_TYPE>
void animation<PIXEL_TYPE>::set_animation_period(double period) {
	fps = 1 / period;
}

template <class PIXEL_TYPE>
int animation<PIXEL_TYPE>::get_animation_period() {
	return 1 / fps;
}

// explicit instantiations
template void animation<wchar_t>::set_animation_speed(double speed);
template int animation<wchar_t>::get_animation_speed();
template void animation<wchar_t>::set_animation_period(double period);
template int animation<wchar_t>::get_animation_period();