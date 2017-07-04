#include <memory>
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <utility>

/*
Still store a pointer to the rendering function in each node,
but use type erasure to provide node interface.
Pros:
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
	It's not possible to access the wrapped renderable object t
	from a generic_node.  This makes it impossible to
	update the wrapped renderable object, except whatever
	update capability is provided by generic_node's interface,
	which is the same for all wrapped types.


*/


#ifndef DEFAULT_RENDERER
#define DEFAULT_RENDERER normal_renderer
#endif

struct renderable_concept {
  virtual void render() const = 0;
};

template<class T>
struct renderable_model : renderable_concept {
  T t;
  std::function<void(const T&)> render_func;
  template<class...Us>
  renderable_model(std::function<void(const T &)> render_func_, Us&&...us ):
  	render_func{render_func_},
    t{std::forward<Us>(us)...}
  {}
  void render() const final override {
	render_func(t);
  }
};

struct generic_node {
  void render() {
    if (!pImpl) return;
    pImpl->render();
  }
  template<class T,class...Us>
  generic_node(std::function<void(const T&)> render_func_, Us&&... us):
    pImpl( std::make_shared<renderable_model<T>>(render_func_, std::forward<Us>(us)...)) {}

private:
  std::shared_ptr<renderable_concept> pImpl;
};

struct node {};

struct rect_node : public node {
	int width;
	int height;
};

struct text_node : public node {
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
generic_node make_node(NODE n) {
	std::function<void(const NODE&)> rndr = static_cast<void(*)(const NODE&)>(RENDERER::render);
	return generic_node{rndr,std::forward<NODE>(n)};
}

int main() {
	auto tn1 = make_node(text_node{"hello"});
	tn1.render();
	auto tn2 = make_node<text_node,normal_renderer>(text_node{"hello"});
	tn2.render();
	auto tn3 = make_node<text_node,caps_renderer>(text_node{"hello"});
	tn3.render();
	auto rn1 = make_node(rect_node{});
	rn1.render();
	auto rn2 = make_node<rect_node,normal_renderer>(rect_node{});
	rn2.render();
}