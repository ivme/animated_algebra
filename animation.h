#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
#include <memory>
#include "image.h"

template <class PIXEL_TYPE>
class animation {
	friend class viewer;
public:
	animation() : frames(std::vector<std::shared_ptr<image<PIXEL_TYPE>>>()) {}
	animation(std::vector<std::shared_ptr<image<PIXEL_TYPE>>> frames) : frames(frames) {}

	typedef typename std::vector<std::shared_ptr<image<PIXEL_TYPE>>>::iterator iterator;
	typedef typename std::vector<std::shared_ptr<image<PIXEL_TYPE>>>::const_iterator const_iterator;
	iterator begin() {return frames.begin();}
	iterator end() {return frames.end();}

	void set_animation_speed(double speed); // speed = frames per second
	int get_animation_speed();
	void set_animation_period(double period); // period = length of time between frames
	int get_animation_period();

	void present(viewer &v) const {v.present(*this);}
	void append_frame(std::shared_ptr<image<PIXEL_TYPE>> f) {frames.push_back(f);}
	size_t frame_count() {return frames.size();}

private:
	std::vector<std::shared_ptr<image<PIXEL_TYPE>>> frames; // must use pointers because image is abstract class
	double fps = 6; // frames per second
};

#endif