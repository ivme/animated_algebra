#ifndef SCENE_H
#define SCENE_H

#include "image.h"
#include "node.h"
#include <list>
#include <memory>

#define LIVEN_SCENE_WIDTH 150
#define LIVEN_SCENE_HEIGHT 100

namespace liven {

class scene : public std::enable_shared_from_this<scene> {
	friend class node;
public:
	scene() : root(nullptr) {}
	std::shared_ptr<node> get_root() const {return root;}
	void set_root(std::shared_ptr<node> r);
	void refresh_scene_locations(); // recomputes scene locations
	std::vector<std::shared_ptr<node>> nodes();
	located<rect,2> bounding_rect() const; 
	
	static located<rect,2> scene_rect_to_pixel_rect(located<rect,2> scene_rect);
private:
	std::shared_ptr<node> root;
	bool scene_locations_are_dirty;
};


class view {
public:
	view() : rectangle(rect(LIVEN_SCENE_WIDTH,LIVEN_SCENE_HEIGHT),point<2>(0,0)), scn(nullptr) {}
	located<rect,2> rectangle;
	std::shared_ptr<scene> scn;

	void view_whole_scene();
	ascii_renderer::image_type render();
};

} // liven

#endif