#include "scene.h"
#include <cmath>

using namespace liven;

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
	root->compute_scene_locations(point<3>(0,0,0));	
	scene_locations_are_dirty = false;
}

void scene::set_root(std::shared_ptr<node> r) {
	r->set_scene(std::weak_ptr<scene>(shared_from_this()));
	root = r;
	scene_locations_are_dirty = true;
}

// view

image_type view::render() {
	if (!scn) {return image_type();}
	auto nodes = scn->nodes();
	std::shared_ptr<image_type> p_img;
	std::shared_ptr<located<image_type,3>> p_located_img;
	layered_image<image_type> layered_img{};
	
	for (auto node_ptr : nodes) {
		if (node_ptr->is_renderable()) {
			p_img = std::make_shared<image_type>(node_ptr->render());
			auto scene_location = node_ptr->get_scene_location();
			int x = std::floor(render_type::scene_x_coordinate_to_pixels(scene_location.x));
			int y = std::floor(render_type::scene_y_coordinate_to_pixels(scene_location.y));
			int z = scene_location.z;
			point<3> pixel_location{x,y,z};
			p_located_img = std::make_shared<located<image_type,3>>(*p_img,pixel_location);
			layered_img.insert(p_located_img);
		}
	}
	return layered_img.flatten_and_crop(scene_rect_to_pixel_rect(rectangle));
}



