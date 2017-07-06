#include "scene.h"
#include "renderer.h"

// scene
const std::vector<std::shared_ptr<node>> &node::get_children() const {
	return children;
}

void add_children_to_nodes(std::shared_ptr<node> n, std::vector<std::shared_ptr<node>> &nodes) {
	for (auto child : n->get_children()) {
		nodes.push_back(child);
		add_children_to_nodes(child,nodes);	
	}
}

std::vector<std::shared_ptr<node>> scene::nodes() {
	std::vector<std::shared_ptr<node>> nodes{};
	nodes.push_back(root);
	add_children_to_nodes(root,nodes);
	return nodes;
}

void scene::refresh_scene_locations() {
	root->compute_scene_locations_(point<3>(0,0,0));	
	scene_locations_are_dirty = false;
}

void scene::set_root(std::shared_ptr<node> r) {
	r->set_scene(std::weak_ptr<scene>(shared_from_this()));
	root = r;
	scene_locations_are_dirty = true;
}

located<rect,2> scene::bounding_rect() const {
	return root->bounding_rect();
}

// view
void view::view_whole_scene() {
	rectangle = scn->bounding_rect();
}
std::shared_ptr<image<wchar_t>> view::render(renderer<ascii_image> &r) {return r.render(*this);}