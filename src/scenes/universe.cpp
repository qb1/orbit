#include "universe.h"

#include <simulation/object.h>
#include <simulation/universe.h>

#include <graphics/camera.h>
#include <graphics/misc_draw.h>
#include <graphics/object.h>

#include <common/universe_definitions.h>

SceneUniverse::~SceneUniverse() = default;

SceneUniverse::SceneUniverse(ObjectLinkCollection* coll)
    : object_links_(coll)
{
}

void SceneUniverse::draw(sf::RenderTarget& target, const GrTransform& tr)
{
    // Draw things in inverse order to keep most important on top
	for (auto it = object_links_->crbegin(); it != object_links_->rend(); ++it) {
		if (not it->primary_simu())
			continue;
		draw_orbit(target, tr, *it->primary_simu(), it->simu(), it->visu().color());
	}
	for (auto it = object_links_->crbegin(); it != object_links_->rend(); ++it) {
		it->visu().draw(target, tr);
	}
	for (auto it = object_links_->crbegin(); it != object_links_->rend(); ++it) {
		it->visu().draw_label(target, tr);
	}

    draw_scale(target, tr);
}
