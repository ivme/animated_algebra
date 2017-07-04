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
	No downcasts are needed in the rendering functions.

	renderable_concepts of different underlying type can
	be stored in the same container (e.g. vector), which
	makes it easy to build a tree out of them -- each one
	could hold a vector of pointers to its children.

	Set of nodes and renderers is still extensible while
	maintaining encapsulation of the library.  User-defined
	node types and renderers no longer need to inherit
	from library types.

Cons:
	In order to access the wrapped renderable object t
	from a pointer to renderable_concept, the user would
	need to downcast to the correct renderable_model subclass.

	Even after downcasting, the user would still need to access
	the wrapped type through the member variable t, which is
	awkward, e.g.
		struct circle {int radius;};
		renderable_model<circle> circle_model{{5}}; // set radius = 5
		renderable_concept *concept_ptr = &circle_model;
		auto recovered_circle = static_cast<*renderable_model<circle>>(concept_ptr);
		recovered_circle->t.radius; // awkward extra t
*/


struct renderable_concept {
  virtual void render() const = 0;
};

template<class T>
struct renderable_model : renderable_concept {
  T t;
  template<class...Us>
  renderable_model(Us&&...us ):
    t{std::forward<Us>(us)...}
  {}
  std::function<void(const T&)> render_func;
  void set_render_func(std::function<void(const T &)> fn) {render_func = fn;}
  void render() const final override {
	render_func(t);
  }
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

void render_text_node_default(const text_node &tn) {
	std::cout << tn.text << std::endl;
}

void render_text_node_caps(const text_node &tn) {
	std::string upper_text = tn.text;
	for (auto &c : upper_text) {c = std::toupper(c);}
	std::cout << upper_text << std::endl;
}


int main() {
	auto rm_tn = renderable_model<text_node>{"hello"};
	rm_tn.set_render_func(render_text_node_default);
	rm_tn.render();
	rm_tn.set_render_func(render_text_node_caps);
	rm_tn.render();
}