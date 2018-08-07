#pragma once

#include <optional>

#include <glm/gtx/rotate_vector.hpp>
#include <imgui.h>

#include <common/computations.h>
#include <common/utils.h>
#include <graphics/camera.h>
#include <graphics/misc_draw.h>

#include "object_link.h"
#include "base.h"

class ShipOrbitParameters : public SceneBase
{
public:
	ShipOrbitParameters(ObjectLinkCollection* coll)
	  : object_links_(coll)
	{
	}

	virtual ~ShipOrbitParameters() = default;

	virtual void draw(sf::RenderTarget& target, const GrTransform& tr)
	{
		const auto& ship_simu = ship_->second;

		ImGui::Begin("Ship Orbits", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        for (const auto& obj : *object_links_) {
        	if (obj.simu_ == ship_)
        		continue;

			auto params = compute_orbit_from_state(obj.simu().position(), obj.simu().velocity(), obj.simu().mass(),
			                                       ship_simu.position(), ship_simu.velocity());
			if (params.e > 10 or obj.name() == "Sun")
				continue;

			ImGui::Separator();
			ImGui::Text("Orbiting: %s", obj.name().c_str());
			ImGui::Separator();

			draw_orbit(target, tr, obj.simu(), ship_simu, obj.visu().color(), 1000);

			auto posv = ship_simu.position() - obj.simu().position();
			auto distance_center = glm::length(posv);
			auto distance_surface =
			  distance_center - obj.visu().longest_distance() / 2 - ship_visu_->longest_distance() / 2;
			auto dir = glm::normalize(posv);
			auto surface_vel =
			  glm::rotate(dir, glm::half_pi<double>()) * obj.simu().angular_velocity() * obj.simu().radius();
			auto velocity = ship_simu.velocity() - obj.simu().velocity();
			auto ground_vel = velocity - surface_vel;
			auto speed_center = distance_to_string(glm::length(velocity)) + "/s";
			auto ground_speed = distance_to_string(glm::length(ground_vel)) + "/s";
			ImGui::LabelText("eccentricity", "%f", params.e);
			ImGui::LabelText("distance to center", "%s", distance_to_string(distance_center).c_str());
			ImGui::LabelText("speed to center", "%s", speed_center.c_str());
			ImGui::LabelText("distance to surface", "%s", distance_to_string(distance_surface).c_str());
			ImGui::LabelText("ground speed", "%s", ground_speed.c_str());

			auto vel_dir = glm::normalize(ground_vel);
			auto vel_value = std::min(glm::length(ground_vel) / 10.0 * tr.pix_to_res(30.0), 30.0);
			draw_screen_arrow(target, obj.visu().color(), tr.to_screen(ship_simu.position()), vel_dir, vel_value, 5.0);
		}
        ImGui::End();
	}

	virtual void set_visible(bool) {}
	virtual void set_takes_input(bool) {}

	void set_ship(std::string name)
	{
		auto obj = object_links_->find(name);
		ship_ = obj.simu_;
		ship_visu_ = &obj.visu();
	}

private:
	ObjectLinkCollection* object_links_;
	SimUniverse::Collection::const_iterator ship_;
	GrObject* ship_visu_;
};