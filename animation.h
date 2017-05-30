#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
#include <memory>
#include "image.h"
#include "viewer.h"

class animation {
	friend class viewer;
public:
	animation() : frames(std::vector<std::shared_ptr<image>>()) {}
	animation(std::vector<std::shared_ptr<image>> frames) : frames(frames) {}

	typedef std::vector<std::shared_ptr<image>>::iterator iterator;
	typedef std::vector<std::shared_ptr<image>>::const_iterator const_iterator;
	iterator begin() {return frames.begin();}
	iterator end() {return frames.end();}

	void set_animation_speed(double speed); // speed = frames per second
	int get_animation_speed();
	void set_animation_period(double period); // period = length of time between frames
	int get_animation_period();

	void present(viewer &v) const {v.present(*this);}

private:
	std::vector<std::shared_ptr<image>> frames; // must use pointers because image is abstract class
	double fps = 2; // frames per second
};

#endif