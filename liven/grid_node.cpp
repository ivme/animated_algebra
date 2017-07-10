#include "grid_node.h"
#include "renderer.h"

using namespace liven;

std::shared_ptr<ascii_image> grid_node::render(renderer<ascii_image> &r) {return r.render(*this);}
std::shared_ptr<ascii_image> grid_node::render(ascii_renderer &r) {return r.render(*this);}