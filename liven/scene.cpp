#include "scene.h"
#include "renderer.h"

// node

node::node() :
	location(0,0,0),
	scene_location(0,0,0),
	children(),
	parent(),
	scn() {}

void node::make_scene_locations_dirty_() {
	std::shared_ptr<scene> s = get_scene().lock();
	if (s) {s->scene_locations_are_dirty = true;}
}

void node::set_location(point<3> l) {
	make_scene_locations_dirty_();
	location = l;
}

void node::set_location(int x, int y, int z) {
	set_location(point<3>(x,y,z));
}

void node::shift(point<3> delta) {
	make_scene_locations_dirty_();
	location += delta;
}

void node::shift(int dx, int dy, int dz) {
	make_scene_locations_dirty_();
	location.x += dx;
	location.y += dy;
	location.z += dz;
}

void node::set_scene(std::weak_ptr<scene> s) {
	scn = s;
	for (auto child : children) {
		child->set_scene(s);
	}
}

void node::set_parent(std::weak_ptr<node> p, bool preserve_scene_location) {
	std::shared_ptr<node> old_parent{};
	if ((old_parent = parent.lock())) {
		old_parent->remove_child(shared_from_this());
	}
	auto new_parent = p.lock();
	if (new_parent) {
		new_parent->children.push_back(shared_from_this());
		if (scn.lock() != new_parent->scn.lock()) {
			set_scene(new_parent->scn);
		}
		if (old_parent && preserve_scene_location) {
			auto old_p_scene_loc = old_parent->get_scene_location();
			auto new_p_scene_loc = new_parent->get_scene_location();
			this->location += old_p_scene_loc - new_p_scene_loc;
		}
	}
	parent = p;
	make_scene_locations_dirty_();
}

std::weak_ptr<node> node::get_parent() {
	return parent;
}

void node::add_child(std::shared_ptr<node> child, bool preserve_scene_location) {
	child->set_parent(std::weak_ptr<node>(shared_from_this()),preserve_scene_location);
}

void node::remove_child(std::shared_ptr<node> child) {
	children.erase(std::remove(children.begin(),children.end(),child),children.end());
}

std::weak_ptr<scene> node::get_scene() {
	if (scn.lock()) {return scn;}
	auto p = parent.lock();
	if (p) {return p->get_scene();}
	return std::weak_ptr<scene>();
}

point<3> node::get_scene_location() {
	auto scn_ = get_scene().lock();
	if (scn_) {
		if (scn_->scene_locations_are_dirty) {
			scn_->refresh_scene_locations();	
		}
		return scene_location;
	} else {
		return point<3>();
	}
}

void node::change_child_spacing(int delta, dimension dim) const {
	for (int i = 0; i < children.size(); ++i) {
		switch(dim) {
		case dimension::x:
			children[i]->shift(i * delta,0); break;
		case dimension::y:
			children[i]->shift(0,i * delta); break;
		}
	}
}

located<rect,2> node::own_bounding_rect() const {
	return located<rect,2>();
}

located<rect,2> node::bounding_rect() const {
	located<rect,2> out = own_bounding_rect();
	for (auto child : children) {
		out = union_bounding_rect(out,child->bounding_rect());
	}
	return out;
}

bool node::is_renderable() const {return false;}
std::shared_ptr<ascii_image> node::render(renderer<ascii_image> &r) const {return r.render(*this);}
std::shared_ptr<ascii_image> node::render(ascii_renderer &r) const {return r.render(*this);}

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