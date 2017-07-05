#ifndef NODE_H
#define NODE_H

#include <memory>
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <utility>
#include "graphics.h"

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

#endif