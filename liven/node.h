#ifndef NODE_H
#define NODE_H

#include <memory>
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <utility>
#include "graphics.h"
#include "image.h"

#ifndef DEFAULT_RENDERER
#define DEFAULT_RENDERER normal_renderer
class normal_renderer;
#endif

class scene;

class renderable_concept {
public:
	virtual std::shared_ptr<const void> ptr_to_wrapped() const = 0;
	virtual std::shared_ptr<image> render() const = 0;
};

template<class WRAPPED>
class renderable_model : renderable_concept {
public:
	renderable_model(std::function<void(const WRAPPED &)> render_func_, std::shared_ptr<WRAPPED> w_):
		render_func{render_func_},
		w{w_}
	{}
	std::shared_ptr<const void> ptr_to_wrapped() const final override {
		return w;
	}
	std::shared_ptr<image> render() const final override {
		render_func(*w);
	}

private:
	std::shared_ptr<WRAPPED> w;
	std::function<void(const WRAPPED&)> render_func;
};

class node : public enable_shared_from_this<node> {
public:
	template<class WRAPPED,class RENDERER = DEFAULWRAPPED_RENDERER>
	node(std::shared_ptr<WRAPPED> w, RENDERER renderer = RENDERER());

	template<class WRAPPED, class RENDERER = DEFAULT_RENDERER>
	node(const WRAPPED &w, RENDERER renderer = RENDERER());

	node() : p_renderable(), location(0,0,0) {}

	// location
	point<3> get_location() const {return location;}
	void set_location(point<3> l) {location = l;}
	void set_location(int x, int y, int z);
	void shift(point<3> delta);  // adds delta to the node's location
	void shift(int dx, int dy, int dz = 0);
	void change_child_spacing(int delta, dimension dim) const;

	// parent and children
	void set_parent(std::weak_ptr<node> p, bool preserve_scene_location = false);
	std::weak_ptr<node> get_parent();
	void add_child(std::shared_ptr<node> child, bool preserve_scene_location = false);
	void remove_child(std::shared_ptr<node> child);
	const std::vector<std::shared_ptr<node>> &get_children() const;

	// scene
	std::weak_ptr<scene> get_scene();
	point<3> get_scene_location(); // return location of lower left corner in the enclosing scene's coordinate system

	located<rect,2> own_bounding_rect() const;  // bounding rectangle of this node, not including children
	located<rect,2> bounding_rect() const ; // bounding rect of this node and its children.

	bool is_renderable() const;
	std::shared_ptr<image> render();

	std::shared_ptr<const void> ptr_to_wrapped();

private:
	std::shared_ptr<renderable_concept> p_renderable;
	point<3> location;
	point<3> scene_location;
	point<3> compute_scene_location();

	std::vector<std::shared_ptr<node>> children;
	std::weak_ptr<node> parent;
	std::weak_ptr<scene> scn; // an invariant should be that all of the children of a node n have the same scn pointer as n
	void set_scene(std::weak_ptr<scene> s); // set this and a children's scn values to s
	void compute_scene_locations_(point<3> parent_location);
	void make_scene_locations_dirty_();
};

#endif