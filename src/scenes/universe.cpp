#include "universe.h"

#include <simulation/object.h>
#include <simulation/universe.h>

#include <graphics/camera.h>
#include <graphics/misc_draw.h>
#include <graphics/object.h>

#include <common/universe_definitions.h>

SceneUniverse::~SceneUniverse() = default;

SceneUniverse::SceneUniverse(const sf::RenderTarget& target)
    : visible_(true)
    , takes_input_(true)
{
}

void SceneUniverse::update(float elapsed)
{
	for (auto& link: object_links_) {
		(*link.visu)->update(link.simu->second.position(), link.simu->second.angle());
	}
}

void SceneUniverse::draw(sf::RenderTarget& target, const GrTransform& tr)
{
    // Draw things in inverse order to keep most important on top
	for (auto it = object_links_.crbegin(); it != object_links_.rend(); ++it) {
		if (not it->primary_simu)
			continue;
		draw_orbit(target, tr, it->primary_simu.value()->second, it->simu->second, (*it->visu)->color());
	}
	for (auto it = object_links_.crbegin(); it != object_links_.rend(); ++it) {
		(*it->visu)->draw(target, tr);
	}
	for (auto it = object_links_.crbegin(); it != object_links_.rend(); ++it) {
		(*it->visu)->draw_label(target, tr);
	}

    draw_scale(target, tr);
}

void SceneUniverse::add_object(SimUniverse::Collection::const_iterator sim_object,
                               std::optional<SimUniverse::Collection::const_iterator> prim_sim_object,
                               std::unique_ptr<GrObject> gr_object)
{
	gr_objects_.push_back(std::move(gr_object));
	object_links_.push_back({sim_object, prim_sim_object, gr_objects_.end() - 1});
}

const GrObject& SceneUniverse::find(SimUniverse::Collection::const_iterator simu) const
{
	for (const auto& link: object_links_) {
		if (link.simu == simu)
			return **link.visu;
	}
	throw std::runtime_error("Cannot find matching GrObject for SimObject");
}