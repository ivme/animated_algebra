#include "aa_renderer.h"
#include "p_rect.h"

template<>
aa_renderer::image_type aa_renderer::render<p_rect>(const p_rect &pr) {
	return render(pr.get_grid());
}