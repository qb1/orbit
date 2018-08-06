#pragma once

#include <optional>
#include <cstdint>
#include <deque>
#include <optional>

#include <glm/vec2.hpp>

#include <graphics/object.h>
#include <simulation/object.h>
#include "object_link.h"
#include "base.h"

class SceneUniverse : public SceneBase
{
public:
	// Keeps the pointer
	SceneUniverse(ObjectLinkCollection* coll);
	virtual ~SceneUniverse();

	virtual void draw(sf::RenderTarget& target, const GrTransform& tr);

	virtual void set_visible(bool visible) { visible_ = visible; }
	virtual void set_takes_input(bool takes_input) { takes_input_ = takes_input; }

private:
	ObjectLinkCollection* object_links_;

	bool takes_input_ = true;
	bool visible_ = true;
};