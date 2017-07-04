#include <memory>
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <utility>

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
#endif

struct renderable_concept {
	virtual std::shared_ptr<const void> ptr_to_wrapped() const = 0;
	virtual void render() const = 0;
};

template<class T>
struct renderable_model : renderable_concept {
  std::shared_ptr<T> t;
  std::function<void(const T&)> render_func;
  template<class...Us>
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

struct generic_node {
  void render() {
    if (!pImpl) return;
    pImpl->render();
  }
  std::shared_ptr<const void> ptr_to_wrapped() {
  	return pImpl->ptr_to_wrapped();
  }
  template<class T,class...Us>
  generic_node(std::function<void(const T&)> render_func_, std::shared_ptr<T> t_) :
    pImpl(std::make_shared<renderable_model<T>>(render_func_, t_)) {}

private:
  std::shared_ptr<renderable_concept> pImpl;
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

template<class NODE,class RENDERER = DEFAULT_RENDERER>
generic_node make_node(std::shared_ptr<NODE> n, RENDERER renderer = RENDERER()) {
	std::function<void(const NODE&)> rndr = static_cast<void(*)(const NODE&)>(RENDERER::render);
	return generic_node{rndr,n};
}

int main() {
	auto tn = std::make_shared<text_node>("hello");
	std::vector<generic_node> nodes{};
	nodes.push_back(make_node(tn));
	nodes.push_back(make_node(tn,normal_renderer{}));
	nodes.push_back(make_node<text_node,normal_renderer>(tn));
	for (generic_node n : nodes) {n.render();}
	tn->text = "world";
	for (generic_node n : nodes) {n.render();}
}