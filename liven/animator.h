#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "animation.h"
#include "ascii_renderer.h"
#include "scene.h"

namespace liven {

class action;

class animator {
public:
	animator();
	~animator();

	virtual void set_up_scene() {}
	// virtual void set_up_actions() {}
	virtual animation<image_type> animate();
	void pause(int frame_count);
	void pause(double seconds);
	void snapshot();
	virtual void will_render_frame() {}
	virtual void did_render_frame() {}
	void set_root_action(std::shared_ptr<action> r);

	view v;

#ifndef PRIVACY_OFF
protected:
#endif
	// render the current root action
	// append resulting frames to a
	void render_action();
	void render_action(std::shared_ptr<action> action_);

	animation<image_type> animation_;
	std::shared_ptr<action> root_action;
	bool remaining_action = false;
	int frame_number = 0;
};

} // liven

#endif