#ifndef NODE_H
#define NODE_H

#include "global_defs.h"

#include <memory>
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <utility>
#include "graphics.h"
#include "image.h"

namespace liven {
class scene;

class renderable_concept {
public:
	virtual std::shared_ptr<const void> ptr_to_wrapped() const = 0;
	virtual image_type render() const = 0;
};

template<class WRAPPED>
class renderable_model : public renderable_concept {
public:
	renderable_model(std::shared_ptr<WRAPPED> w_):
		w{w_}
	{}
	std::shared_ptr<const void> ptr_to_wrapped() const final override {
		return w;
	}
	image_type render() const final override {
		return render_type::render(*w);
	}

private:
	std::shared_ptr<WRAPPED> w;
};

class node : public std::enable_shared_from_this<node> {
	friend render_type;
	friend scene;
	
public:
	// constructors
	template<class WRAPPED>
	node(std::shared_ptr<WRAPPED> w) :
		p_renderable(std::make_shared<renderable_model<WRAPPED>>(w)),
		location(0,0,0),
		scene_location(0,0,0),
		children(),
		parent(),
		scn()
	{}

	template<class WRAPPED>
	node(const WRAPPED &w) :
			node(std::make_shared<WRAPPED>(w)) {}

	node() :
		p_renderable(),
		location(0,0,0), 
		scene_location(0,0,0),
		children(),
		parent(),
		scn()
	{}

	// location
	point<3> get_location() const {return location;}
	void set_location(point<3> l);
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

	bool is_renderable() const;
	image_type render();

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
	void compute_scene_locations(point<3> parent_location);
	void make_scene_locations_dirty();
};

} // namespace liven

#endif