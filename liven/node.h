#ifndef NODE_H
#define NODE_H

#include <memory>
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <utility>

// trick for quotes, see https://stackoverflow.com/a/6671729/4425924
#define Q(x) #x
#define QUOTE(x) Q(x)

#define LIVEN_DEFAULT_RENDER_TYPE liven::default_renderer
#define LIVEN_DEFAULT_RENDER_TYPE_HEADER_PATH default_renderer.h

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
#define LIVEN_RENDER_TYPE LIVEN_DEFAULT_RENDER_TYPE
#include QUOTE(LIVEN_DEFAULT_RENDER_TYPE_HEADER_PATH)
#endif

namespace liven {

using render_type = LIVEN_RENDER_TYPE;
using image_type = render_type::image_type;

struct renderable_concept {
	virtual image_type render() const = 0;
};

template<class WRAPPED>
struct renderable_model : renderable_concept {
	WRAPPED *w;
	renderable_model(WRAPPED *w_):
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
	node(WRAPPED *w) :
		p_renderable(std::make_shared<renderable_model<WRAPPED>>(w))
	{}

	node() : p_renderable() {}

private:
	std::shared_ptr<renderable_concept> p_renderable;
};

struct rect_node : public node {
	rect_node(int w, int h) : node(this), width(w), height(h) {}
	int width;
	int height;
};

struct text_node : public node {
	text_node(std::string text_) : node(this), text(text_) {}
	std::string text;
};

} // </namespace liven>

#endif
