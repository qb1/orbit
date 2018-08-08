#pragma once

#include <optional>
#include <imgui.h>

#include <graphics/camera.h>

#include "object_link.h"
#include "base.h"

class SceneCenterOn : public SceneBase
{
public:
	SceneCenterOn(ObjectLinkCollection* coll, GrCamera* camera, int* simu_speed_factor)
	  : object_links_(coll)
	  , camera_(camera)
	  , simu_speed_factor_(simu_speed_factor)
	{
	}

	virtual ~SceneCenterOn() = default;

	virtual void update(float)
	{
		ImGui::Begin("Control.", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

		const char* current = center_camera_on_ ? center_camera_on_.value()->first.c_str() : nullptr;
		if (ImGui::BeginCombo("Center On", current)) {
	        for (const auto& obj : *object_links_) {
	        	bool checked = false;
	        	if (center_camera_on_ and center_camera_on_.value() == obj.simu_) {
	        		checked = true;
	        	}
	        	bool was_checked = checked;
	        	ImGui::Selectable(obj.name().c_str(), &checked);

	        	if (was_checked != checked) {
	        		if (checked) {
	        			center_camera_on_ = obj.simu_;
						camera_->set_viewable_distance(obj.simu().radius() * 2.4);
	        			camera_->position = glm::dvec2();
					} else {
	        			center_camera_on_ = std::nullopt;
	        		}
	        	}
	        }
			ImGui::EndCombo();
		}

		auto simu_minus = ImGui::Button("-");
		ImGui::SameLine();
		ImGui::SliderInt("", simu_speed_factor_, 1, 1024 * 256);
		ImGui::SameLine();
		auto simu_plus = ImGui::Button("+");
		ImGui::SameLine();
		ImGui::Text("Simulation speed");

		ImGui::End();

		if (center_camera_on_) {
			camera_->origin = center_camera_on_.value()->second.position();
		}
		if (simu_minus)
			*simu_speed_factor_ /= 2;
		if (simu_plus)
			*simu_speed_factor_ *= 2;
		*simu_speed_factor_ = std::min(*simu_speed_factor_, 1024 * 256);
		*simu_speed_factor_ = std::max(*simu_speed_factor_, 1);
	}

	virtual void set_visible(bool) {}
	virtual void set_takes_input(bool) {}

	const SimObject* camera_centered_on() { return center_camera_on_ ? &center_camera_on_.value()->second : nullptr; }
	void center_camera_on(std::string name) { center_camera_on_ = object_links_->find(name).simu_; }

private:
	ObjectLinkCollection* object_links_;
	std::optional<SimUniverse::Collection::const_iterator> center_camera_on_;
	GrCamera* camera_;
	int* simu_speed_factor_;
};