#pragma once

#include <optional>
#include <cstdint>
#include <deque>
#include <optional>

#include <glm/vec2.hpp>

#include <common/universe_definitions.h>
#include <graphics/object.h>
#include <simulation/universe.h>
#include <scenes/base.h>

class SceneUniverse : public SceneBase
{
public:
	SceneUniverse(const sf::RenderTarget& target);
	virtual ~SceneUniverse();

	virtual void update(float elapsed);
	virtual void draw(sf::RenderTarget& target, const GrTransform& tr);

	virtual void set_visible(bool visible) { visible_ = visible; }
	virtual void set_takes_input(bool takes_input) { takes_input_ = takes_input; }

	void add_object(SimUniverse::Collection::const_iterator sim_object,
	                std::optional<SimUniverse::Collection::const_iterator> prim_sim_object,
	                std::unique_ptr<GrObject> gr_object);

	const GrObject& find(SimUniverse::Collection::const_iterator simu) const;

private:
	using GrObjectCollection = std::deque<std::unique_ptr<GrObject>>;
	GrObjectCollection gr_objects_;

	struct ObjectLink
	{
		SimUniverse::Collection::const_iterator simu;
		std::optional<SimUniverse::Collection::const_iterator> primary_simu;
		GrObjectCollection::iterator visu;
	};

	std::vector<ObjectLink> object_links_;

	bool takes_input_;
	bool visible_;
};