#include <memory>
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <utility>

// trick for quotes, see https://stackoverflow.com/a/6671729/4425924
#define Q(x) #x
#define QUOTE(x) Q(x)

// if the user compiling liven has specified a renderer
// and a path to the header containing the declaration
// of the renderer, use it.  otherwise use the default
// renderer.

#ifdef LIVEN_RENDER_TYPE
	#ifndef LIVEN_RENDER_TYPE_HEADER_PATH
		#error Render class specified, but path to render class header not specified.
	#else
	#include QUOTE(LIVEN_RENDER_TYPE_HEADER_PATH)
	#endif
#else
#define LIVEN_RENDER_TYPE liven::default_renderer
#include "default_renderer.h"
#endif

namespace liven {

using render_type = LIVEN_RENDER_TYPE;
using image_type = render_type::image_type;

struct renderable_concept {
	virtual image_type render() const = 0;
};

template<class WRAPPED>
struct renderable_model : renderable_concept {
	std::shared_ptr<WRAPPED> w;
	renderable_model(std::shared_ptr<WRAPPED> w_):
		w{w_}
	{}
	image_type render() const final override {
		return render_type::render(*w);
	}
};

class node {
public:
	image_type render() {
		if (!p_renderable) {throw std::runtime_error("cannot render node. p_renderable is null.");}
		return p_renderable->render();
	}

	template<class WRAPPED>
	node(std::shared_ptr<WRAPPED> w) :
		p_renderable(std::make_shared<renderable_model<WRAPPED>>(w))
	{}

	template<class WRAPPED>
	node(const WRAPPED &w) :
		node(std::make_shared<WRAPPED>(w)) {}

	node() : p_renderable() {}

private:
	std::shared_ptr<renderable_concept> p_renderable;
};

} // </namespace liven>

int main() {
	liven::text_node t{"hello"};
	std::vector<liven::node> nodes;
	nodes.push_back(liven::text_node{"hello"});
	nodes.push_back(liven::rect_node{1,2});
	for (auto n : nodes) {n.render();}

}