#ifndef SCENE_H
#define SCENE_H

#include "image.h"
#include <list>
#include <memory>

template <class PIXEL_TYPE>
class renderer;
class ascii_renderer;
class scene;

class node : public std::enable_shared_from_this<node> {
	friend class scene;

public:
	// location of lower left corner of node in parent's coordinate system
	// more positive z-locations are closer to foreground
	point<3> get_location() const {return location;}
	void set_location(point<3> l);
	void shift(point<3> delta);  // adds delta to the node's location
	void shift(int dx, int dy, int dz);

	void set_parent(std::weak_ptr<node> p);
	std::weak_ptr<node> get_parent();

	void add_child(std::shared_ptr<node> child);
	void remove_child(std::shared_ptr<node> child);
	const std::vector<std::shared_ptr<node>> &get_children() const;

	std::weak_ptr<scene> get_scene();

	point<3> get_scene_location(); // return location of lower left corner in the enclosing scene's coordinate system
	virtual std::shared_ptr<ascii_image> render(renderer<ascii_image> &r) const = 0;
	virtual std::shared_ptr<ascii_image> render(ascii_renderer &r) const = 0;

private:
	point<3> location;
	
	point<3> scene_location;
	// bool scene_location_is_dirty;
	point<3> compute_scene_location();

	std::set<std::shared_ptr<node>> children;
	std::weak_ptr<node> parent;
	std::weak_ptr<scene> scn; // an invariant should be that all of the children of a node n have the same scn pointer as n
	void compute_scene_locations_(point<3> parent_location);
};

class scene : public std::enable_shared_from_this<scene> {
	friend class node;
public:
	scene() : root(nullptr) {}
	std::shared_ptr<const node> get_root() const {return root;}
	void set_root(std::shared_ptr<node> r);
	void refresh_scene_locations(); // recomputes scene locations
private:
	std::shared_ptr<node> root;
	bool scene_locations_are_dirty;
};

class view {
public:
	located<rect,2> rectangle;
	std::shared_ptr<image<wchar_t>> render(renderer<ascii_image> &r);
	std::weak_ptr<scene> scn;
};

#endif