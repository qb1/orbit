#pragma once

#include <optional>
#include <imgui.h>

#include <graphics/camera.h>

#include "object_link.h"
#include "base.h"

class SceneCenterOn : public SceneBase
{
public:
	SceneCenterOn(ObjectLinkCollection* coll, GrCamera* camera)
	  : object_links_(coll)
	  , camera_(camera)
	{
	}

	virtual ~SceneCenterOn() = default;

	virtual void update(float)
	{
		ImGui::Begin("Ref. obj.", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        for (const auto& obj : *object_links_) {
        	bool checked = false;
        	if (center_camera_on_ and center_camera_on_.value() == obj.simu_) {
        		checked = true;
        	}
        	bool was_checked = checked;
        	ImGui::Checkbox(obj.name().c_str(), &checked);

        	if (was_checked != checked) {
        		if (checked) {
        			center_camera_on_ = obj.simu_;
					camera_->set_viewable_distance(obj.visu().longest_distance() * 1.2);
				} else {
        			center_camera_on_ = std::nullopt;
        		}
        	}
        }
        ImGui::End();

		if (center_camera_on_) {
			camera_->origin = center_camera_on_.value()->second.position();
		}
	}

	virtual void set_visible(bool) {}
	virtual void set_takes_input(bool) {}

	const SimObject* camera_centered_on() { return center_camera_on_ ? &center_camera_on_.value()->second : nullptr; }
	void center_camera_on(std::string name) { center_camera_on_ = object_links_->find(name).simu_; }

private:
	ObjectLinkCollection* object_links_;
	std::optional<SimUniverse::Collection::const_iterator> center_camera_on_;
	GrCamera* camera_;
};