#include <memory>
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <utility>
#include "liven/graphics.h"

/*
Use type erasure for node interface, but store the wrapped
object by reference via a shared_ptr.
Pros:
	If we store a pointer to the wrapped object prior to
	node constructed, we still have access to the wrapped
	object in a reasonably natural way (see tn in main()).
	Given a generic_node, we can examine the address of
	the wrapped object via ptr_to_wrapped -- even if we
	don't know the type of the wrapped object.
	Easy to make a templated factory function that selects the
	correct rendering function via overload resolution.
	Also easy to group rendering functions into rendering
	classes, and to leverage library rendering functionality
	in a custom renderer (see caps_renderer) for all but a few 
	special cases (see template specialization of
	caps_renderer::render<text_node>).
	No downcasts are needed in the rendering functions.
	generic_nodes of different wrapped types can
	be stored in the same container (e.g. vector), which
	makes it easy to build a tree out of them -- each one
	could hold a vector of pointers to its children.
	Set of nodes and renderers is still extensible while
	maintaining encapsulation of the library.  User-defined
	node types and renderers no longer need to inherit
	from library types.
Cons:
	It's necessary to store a pointer to the wrapped object
	before node construction in order to be able to access
	the wrapped object without downcasting.  This is awkward
	because the user needs to manually keep track of which 
	pointers point to objects wrapped by which generic_nodes.
	This could quickly become tedious.
*/

#ifndef DEFAULT_RENDERER
#define DEFAULT_RENDERER normal_renderer
class normal_renderer;
#endif

struct renderable_concept {
	virtual std::shared_ptr<const void> ptr_to_wrapped() const = 0;
	virtual void render() const = 0;
};

template<class T>
struct renderable_model : renderable_concept {
	std::shared_ptr<T> t;
	std::function<void(const T&)> render_func;

	renderable_model(std::function<void(const T &)> render_func_, std::shared_ptr<T> t_):
		render_func{render_func_},
		t{t_}
	{}
	std::shared_ptr<const void> ptr_to_wrapped() const final override {
		return t;
	}
	void render() const final override {
		render_func(*t);
	}
};

struct node {
	point<3> get_location() const {return location;}
	void set_location(point<3> l) {location = l;}

	void render() {
		if (p_renderable) {p_renderable->render();}
	}
	std::shared_ptr<const void> ptr_to_wrapped() {
		return p_renderable->ptr_to_wrapped();
	}

	template<class WRAPPED,class RENDERER = DEFAULT_RENDERER>
	node(std::shared_ptr<WRAPPED> w, RENDERER renderer = RENDERER()) :
		p_renderable(std::make_shared<renderable_model<WRAPPED>>(
			std::function<void(const WRAPPED&)>(static_cast<void(*)(const WRAPPED&)>(RENDERER::render)),
			w)
		),
		location(0,0,0)
	{}

	template<class WRAPPED, class RENDERER = DEFAULT_RENDERER>
	node(const WRAPPED &w, RENDERER renderer = RENDERER()) :
		node(std::make_shared<WRAPPED>(w), renderer) {}

	node() : p_renderable(), location(0,0,0) {}

private:
	std::shared_ptr<renderable_concept> p_renderable;
	point<3> location;
};

struct rect_node {
	int width;
	int height;
};

struct text_node {
	text_node(std::string text_) : text(text_) {}
	std::string text;
};

struct normal_renderer {
	static void render(const rect_node &rn) {std::cout << rn.width << "," << rn.height << std::endl;}
	static void render(const text_node &tn) {std::cout << tn.text << std::endl;}
};

struct caps_renderer {
	template<class T>
	static void render(const T &rn) {normal_renderer::render(rn);}
};

// explicitly specialize caps_renderer to render text_nodes in all caps	
template<>
void caps_renderer::render<text_node>(const text_node &tn) {
	std::string upper_text = tn.text;
	for (auto &c : upper_text) {c = std::toupper(c);}
	std::cout << upper_text << std::endl;
}

int main() {
	auto tn = std::make_shared<text_node>("hello");
	std::vector<node> nodes{};
	nodes.push_back(node(tn));
	nodes.push_back(node(tn,normal_renderer{}));
	nodes.push_back(node(tn,caps_renderer{}));
	nodes.push_back(node(text_node{"again"},caps_renderer{}));
	for (node n : nodes) {n.render();}
	tn->text = "world";
	for (node n : nodes) {n.render();}
}