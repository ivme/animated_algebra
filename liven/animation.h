#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
#include <memory>
#include "image.h"

namespace liven {

template <class IMAGE_TYPE>
class animation {
	friend class viewer<IMAGE_TYPE>;
public:
	animation() : frames(std::vector<IMAGE_TYPE>()) {}
	animation(std::vector<IMAGE_TYPE> frames) : frames(frames) {}

	typedef typename std::vector<IMAGE_TYPE>::iterator iterator;
	typedef typename std::vector<IMAGE_TYPE>::const_iterator const_iterator;
	iterator begin() {return frames.begin();}
	iterator end() {return frames.end();}

	void set_animation_speed(double speed); // speed = frames per second
	int get_animation_speed();
	void set_animation_period(double period); // period = length of time between frames
	int get_animation_period();

	void append_frame(IMAGE_TYPE f) {frames.push_back(f);}
	size_t frame_count() {return frames.size();}

private:
	std::vector<IMAGE_TYPE> frames; // must use pointers because image is abstract class
	double fps = 10; // frames per second
};

template <class IMAGE_TYPE>
void animation<IMAGE_TYPE>::set_animation_speed(double speed) {
	fps = speed;
}

template <class IMAGE_TYPE>
int animation<IMAGE_TYPE>::get_animation_speed() {
	return fps;
}

template <class IMAGE_TYPE>
void animation<IMAGE_TYPE>::set_animation_period(double period) {
	fps = 1 / period;
}

template <class IMAGE_TYPE>
int animation<IMAGE_TYPE>::get_animation_period() {
	return 1 / fps;
}

} // liven

#endif