#ifndef NODE_H
#define NODE_H

// trick for quotes, see https://stackoverflow.com/a/6671729/4425924
#define Q(x) #x
#define QUOTE(x) Q(x)

#define LIVEN_DEFAULT_RENDER_TYPE liven::ascii_renderer
#define LIVEN_DEFAULT_RENDER_TYPE_HEADER_PATH ascii_renderer.h

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

class scene;

class node : public std::enable_shared_from_this<node> {
	friend class scene;

public:
	template<class WRAPPED>
	node(WRAPPED *w) :
		p_renderable(std::make_shared<renderable_model<WRAPPED>>(w)),
		location(point<3>(0,0,0)),
		scene_location(0,0,0),
		children(),
		parent(),
		scn()
	{}

	node();

	image_type render() {
		if (!p_renderable) {throw std::runtime_error("cannot render node. p_renderable is null.");}
		return p_renderable->render();
	}

	// location of lower left corner of node in parent's coordinate system
	// more positive z-locations are closer to foreground
	point<3> get_location() const {return location;}

	// moving methods
	void set_location(point<3> l);
	void set_location(int x, int y, int z);
	void shift(point<3> delta);  // adds delta to the node's location
	void shift(int dx, int dy, int dz = 0);
	void change_child_spacing(int delta, dimension dim) const;

	void set_parent(std::weak_ptr<node> p, bool preserve_scene_location = false);
	std::weak_ptr<node> get_parent();

	void add_child(std::shared_ptr<node> child, bool preserve_scene_location = false);
	void remove_child(std::shared_ptr<node> child);
	const std::vector<std::shared_ptr<node>> &get_children() const;

	std::weak_ptr<scene> get_scene() const;
	// return location of lower left corner in the enclosing scene's coordinate system
	// if the node is not associated with a scene, calls get_location()
	point<3> get_scene_location() const; 

	// bounding rectangle of this node, not including children, in scene coordinates
	virtual located<rect,2> own_bounding_rect() const;  
	located<rect,2> bounding_rect() const ; // bounding rect of this node and its children.

	bool is_renderable() const {return (bool)p_renderable;}
	bool is_visible = true;

private:
	std::shared_ptr<renderable_concept> p_renderable;
	point<3> location = point<3>(0,0,0);
	
	point<3> scene_location;
	// bool scene_location_is_dirty;
	point<3> compute_scene_location();

	std::vector<std::shared_ptr<node>> children;
	std::weak_ptr<node> parent;
	std::weak_ptr<scene> scn; // an invariant should be that all of the children of a node n have the same scn pointer as n
	void set_scene(std::weak_ptr<scene> s); // set this and a children's scn values to s
	void compute_scene_locations_(point<3> parent_location);
	void make_scene_locations_dirty_();
};

} // liven

#endif