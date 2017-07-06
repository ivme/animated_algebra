#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
#include <memory>
#include "global_defs.h"
#include "image.h"

namespace liven {

class animation {
	friend class viewer;
public:
	animation() : frames(std::vector<image_type>()) {}
	animation(std::vector<image_type> frames) : frames(frames) {}

	typedef typename std::vector<image_type>::iterator iterator;
	typedef typename std::vector<image_type>::const_iterator const_iterator;
	iterator begin() {return frames.begin();}
	iterator end() {return frames.end();}

	void set_animation_speed(double speed); // speed = frames per second
	int get_animation_speed();
	void set_animation_period(double period); // period = length of time between frames
	int get_animation_period();

	template <class VIEWER>
	void present(VIEWER &v);
	void append_frame(image_type f) {frames.push_back(f);}
	size_t frame_count() {return frames.size();}

private:
	std::vector<image_type> frames; // must use pointers because image is abstract class
	double fps = 10; // frames per second
};

}

#endif