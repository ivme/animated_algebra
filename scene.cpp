#include "scene.h"
#include "renderer.h"

// node

void node::set_location(point<3> l) {
	// update scene_location
	get_scene().lock()->scene_locations_are_dirty = true;
	location = l;
}

void node::shift(point<3> delta) {
	get_scene().lock()->scene_locations_are_dirty = true;
	location += delta;
}

void node::shift(int dx, int dy, int dz) {
	get_scene().lock()->scene_locations_are_dirty = true;
	location.x += dx;
	location.y += dy;
	location.z += dz;
}

void node::set_parent(std::weak_ptr<node> p) {
	get_scene().lock()->scene_locations_are_dirty = true;
	scn = p.lock()->get_scene();
	parent = p;
}

std::weak_ptr<node> node::get_parent() {
	return parent;
}

void node::add_child(std::shared_ptr<node> child) {
	get_scene().lock()->scene_locations_are_dirty = true;
	child->scn = get_scene();
	children.insert(child);
}

void node::remove_child(std::shared_ptr<node> child) {
	children.erase(child);
}

std::weak_ptr<scene> node::get_scene() {
	return scn;
}

point<3> node::get_scene_location() {
	auto scn_ = get_scene().lock();
	if (scn_->scene_locations_are_dirty) {
		scn_->refresh_scene_locations();	
	}
	return scene_location;
}

// recursive utility function
void node::compute_scene_locations_(point<3> parent_location) {
	scene_location = parent_location + location;
	for (auto child : children) {
		child->compute_scene_locations_(scene_location);
	}
}


// compare_z (class def in graphics.h)
bool compare_z::operator()(const std::shared_ptr<node> &a, const std::shared_ptr<node> &b) {
	return a->get_location().z < b->get_location().z;
}

// scene
const std::set<std::shared_ptr<node>> &node::get_children() {
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
	r->scn = std::weak_ptr<scene>(shared_from_this());
	root = r;
}

// view
std::shared_ptr<image<wchar_t>> view::render(renderer<ascii_image> &r) {return r.render(*this);}