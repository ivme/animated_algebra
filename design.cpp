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
  std::function<void(const T&)> render_func;
  template<class...Us>
  renderable_model(std::function<void(const T &)> render_func_, Us&&...us ):
  	render_func{render_func_},
    t{std::forward<Us>(us)...}
  {}
  void set_render_func(std::function<void(const T &)> fn) {render_func = fn;}
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

struct node;
struct text_node;
struct rect_node;
void render_text_node_default(const text_node &tn);
void render_text_node_caps(const text_node &tn);

struct node {};

struct rect_node : public node {
	int width;
	int height;
};

struct text_node : public node {
	text_node(std::string text_) : text(text_) {}
	std::string text;
	static constexpr void (*default_render)(const text_node&) = render_text_node_default;
};

void render_text_node_default(const text_node &tn) {
	std::cout << tn.text << std::endl;
}

void render_text_node_caps(const text_node &tn) {
	std::string upper_text = tn.text;
	for (auto &c : upper_text) {c = std::toupper(c);}
	std::cout << upper_text << std::endl;
}

template<class T>
generic_node make_node(T t, void (*render_func_)(const text_node&) = T::default_render) {
	return generic_node{std::function<void(const T&)>(render_func_),std::forward<T>(t)};
}

int main() {
	auto tn1 = make_node(text_node{"hello"});
	tn1.render();
	auto tn2 = make_node(text_node{"hello"},render_text_node_caps);
	tn2.render();
}