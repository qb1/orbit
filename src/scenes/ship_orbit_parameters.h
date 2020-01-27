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

	void update(float elapsed) override
	{
		if (ship_orientation_follows_) {
			const auto& prim_simu = ship_orientation_follows_.value()->second;
			auto& ship_simu = ship_->second;
			auto orbit = compute_orbit_from_state(prim_simu.position(), prim_simu.velocity(), prim_simu.mass(),
			                                      ship_simu.position(), ship_simu.velocity());
			auto v = true_anomaly_from_primary(orbit, prim_simu.position(), ship_simu.position());
			auto flight_angle = flight_path_angle(orbit, v);

			auto zenith_angle = angle_from_primary(prim_simu.position(), ship_simu.position());
			// Assume counter-clockwise orbit
			zenith_angle -= glm::half_pi<double>();
			ship_simu.set_angle(zenith_angle - flight_angle);
		}
	}

	void draw(sf::RenderTarget& target, const GrTransform& tr) override
	{
		const auto& ship_simu = ship_->second;

		ImGui::Begin("Ship Orbits", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        for (const auto& obj : *object_links_) {
        	if (obj.simu_ == ship_)
        		continue;

			auto orbit = compute_orbit_from_state(obj.simu().position(), obj.simu().velocity(), obj.simu().mass(),
			                                      ship_simu.position(), ship_simu.velocity());
			if (orbit.e > 10)
				continue;

			ImGuiTreeNodeFlags flags = 0;
			if (obj.name() != "Sun")
				flags = ImGuiTreeNodeFlags_DefaultOpen;

			if (ImGui::CollapsingHeader(obj.name().c_str(), flags)) {

				draw_orbit(target, tr, obj.simu(), ship_simu, obj.visu().color(), 1000);

				auto v = true_anomaly_from_primary(orbit, obj.simu().position(), ship_simu.position());
				auto relative_position = ship_simu.position() - obj.simu().position();
				auto distance_center = glm::length(relative_position);
				auto distance_surface =
				  distance_center - obj.simu().radius() - ship_simu.radius();
				auto dir = glm::normalize(relative_position);
				auto rotation_dir = glm::rotate(dir, glm::half_pi<double>());
				auto surface_vel =
				  glm::rotate(dir, glm::half_pi<double>()) * obj.simu().angular_velocity() * distance_center;
				auto rel_velocity = ship_simu.velocity() - obj.simu().velocity();
				auto center_vel = glm::dot(rel_velocity, dir);
				auto ground_vel = rel_velocity - surface_vel;
				auto speed_relative = distance_to_string(glm::length(rel_velocity)) + "/s";
				auto speed_center = distance_to_string(center_vel) + "/s";
				auto ground_speed = distance_to_string(std::abs(glm::dot(ground_vel, rotation_dir))) + "/s";
				ImGui::LabelText("eccentricity", "%f", orbit.e);
				ImGui::LabelText("true anomaly", "%f", to_degrees(v));
				ImGui::LabelText("distance from center", "%s", distance_to_string(distance_center).c_str());
				ImGui::LabelText("distance to surface", "%s", distance_to_string(distance_surface).c_str());
				ImGui::LabelText("relative speed", "%s", speed_relative.c_str());
				ImGui::LabelText("speed to center", "%s", speed_center.c_str());
				ImGui::LabelText("ground speed", "%s", ground_speed.c_str());

				auto vel_to_cir = velocity_for_circular_orbit(obj.simu().position(), obj.simu().velocity(),
					                                          obj.simu().mass(), ship_simu.position());
				auto cir_speed_delta = distance_to_string(glm::length(vel_to_cir - ship_simu.velocity())) + "/s";
				ImGui::LabelText("dV to circular orbit", "%s", cir_speed_delta.c_str());

				{
					glm::dvec2 vector;
					if (arrow_velocity_selected_ == 0) {
						vector = ground_vel;
					} else if(arrow_velocity_selected_ == 1) {
						vector = vel_to_cir - ship_simu.velocity();
					}

					auto length = std::min(glm::length(vector) / 10.0 * tr.pix_to_res(30.0), tr.pix_to_res(30.0));
					if (length > 1.0) {
						vector = glm::normalize(vector) * length;
						draw_screen_arrow(target, obj.visu().color(), tr.to_screen(ship_simu.position()), vector,
						                  tr.pix_to_res(5.0));
					}
				}

				bool follow = ship_orientation_follows_ && ship_orientation_follows_.value() == obj.simu_;
				bool was_follow = follow;
				auto button_id = std::string("Sync ship orientation##") + obj.name();
				ImGui::Checkbox(button_id.c_str(), &follow);
				if (follow != was_follow) {
					if (follow)
						ship_orientation_follows_ = obj.simu_;
					else
						ship_orientation_follows_ = std::nullopt;
				}
			}

		}

		ImGui::Separator();
		ImGui::Text("Display velocity vector:");
		ImGui::RadioButton("Ground velocity", &arrow_velocity_selected_, 0);
		ImGui::RadioButton("dV to circular orbit", &arrow_velocity_selected_, 1);
        ImGui::End();
	}

	void set_visible(bool) override {}
	void set_takes_input(bool) override {}

	void set_ship(SimUniverse::Collection::iterator ship)
	{
		ship_ = ship;
		ship_orientation_follows_ = std::nullopt;
	}

private:
	ObjectLinkCollection* object_links_;
	SimUniverse::Collection::iterator ship_;
	std::optional<SimUniverse::Collection::const_iterator> ship_orientation_follows_;
	int arrow_velocity_selected_ = 0;
};