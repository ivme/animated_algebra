#ifndef SCENE_H
#define SCENE_H

#include "image.h"
#include "node.h"
#include <list>
#include <memory>

namespace liven {

using image_type = render_type::image_type;

class scene : public std::enable_shared_from_this<scene> {
	friend class node;
public:
	scene() : root(nullptr) {}
	std::shared_ptr<node> get_root() const {return root;}
	void set_root(std::shared_ptr<node> r);
	void refresh_scene_locations(); // recomputes scene locations
	std::vector<std::shared_ptr<node>> nodes();

private:
	std::shared_ptr<node> root;
	bool scene_locations_are_dirty;
};

class view {
public:
	view() : rectangle(rect(170,100),point<2>(0,0)), scn(nullptr) {}
	located<rect,2> rectangle;
	std::shared_ptr<scene> scn;

	image_type render();
};

}

#endif