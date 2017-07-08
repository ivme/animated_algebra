#include "aa_renderer.h"
#include "p_rect.h"

aa_renderer::image_type aa_renderer::render(const p_rect &pr) {
	return render(pr.get_grid());
}