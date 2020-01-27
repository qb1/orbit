#pragma once

#include <optional>

#include <glm/gtc/constants.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <imgui.h>
#include <SFML/Graphics/CircleShape.hpp>

#include <common/computations.h>
#include <common/utils.h>
#include <graphics/camera.h>
#include <graphics/misc_draw.h>
#include <graphics/shapes_routines.h>
#include <graphics/utils.h>

#include "object_link.h"
#include "base.h"
#include "utils.h"

class OrbitPrediction : public SceneBase
{
public:
	OrbitPrediction(ObjectLinkCollection* coll)
	  : object_links_(coll)
	{
	}

	virtual ~OrbitPrediction() = default;

	void draw(sf::RenderTarget& target, const GrTransform& tr) override
	{
		ImGui::Begin("Orbit Prediction", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

		if (imgui_select_object("Selected Primary", selected_primary_, *object_links_, ship_)) {
			if (selected_primary_) {
				mouse_selecting_anomaly_ = true;
			}
		}
		if (not selected_primary_) {
			ImGui::End();
			return;
		}
		auto& ship_simu = ship_->second;
		const auto& prim_simu = selected_primary_.value()->second;

		if (imgui_select_object("Selected destination (optional)", selected_hohmann_, *object_links_, ship_)) {
			if (selected_hohmann_) {
				mouse_v_ = glm::pi<double>();
				mouse_selecting_anomaly_ = false;
			}
		}

		if (selected_hohmann_) {
			const auto& dest_simu = selected_hohmann_.value()->second;
			auto dest_orbit = compute_orbit_from_state(prim_simu.position(), prim_simu.velocity(), prim_simu.mass(),
			                                           dest_simu.position(), dest_simu.velocity());
			double ship_angle = angle_from_primary(prim_simu.position(), ship_simu.position());
			double dest_angle = ship_angle + glm::pi<double>();
			double dest_v = dest_angle - dest_orbit.longitude;
			double dest_radius = orbit_radius(dest_orbit, dest_v);

			double init_radius = glm::length(ship_simu.position() - prim_simu.position());

			double tx_a = (init_radius + dest_radius) / 2;

			double init_tx_speed = glm::sqrt(grav_const * prim_simu.mass() * (2 / init_radius - 1 / tx_a));
			double final_tx_speed = glm::sqrt(grav_const * prim_simu.mass() * (2 / dest_radius - 1 / tx_a));

			ImGui::LabelText("hohmann initial velocity", "%s /s", distance_to_string(init_tx_speed).c_str());
			ImGui::LabelText("hohmann final velocity", "%s /s", distance_to_string(final_tx_speed).c_str());

			// Assume counter-clockwise
			auto zenith = glm::rotate(glm::normalize(ship_simu.position() - prim_simu.position()), -glm::half_pi<double>());
			auto init_tx_vel = zenith * init_tx_speed;
			auto delta_vel = init_tx_vel - (ship_simu.velocity() - prim_simu.velocity());
			ship_simu.set_angle(glm::orientedAngle(glm::dvec2(1.0, 0.0), glm::normalize(delta_vel)));
			delta_v_ = glm::length(delta_vel);
		}

		ImGui::Separator();

		ImGui::LabelText("dV to add", "%s /s", distance_to_string(delta_v_).c_str());

		if (ImGui::Button("1k")) delta_v_ -= 1000.0;
		ImGui::SameLine();
		if (ImGui::Button("100")) delta_v_ -= 100.0;
		ImGui::SameLine();
		if (ImGui::Button("10")) delta_v_ -= 10.0;
		ImGui::SameLine();
		if (ImGui::Button("1")) delta_v_ -= 1.0;
		ImGui::SameLine();
		if (ImGui::Button("Zero##Prediction")) delta_v_ = 0.0;
		ImGui::SameLine();
		if (ImGui::Button("1##+1")) delta_v_ += 1.0;
		ImGui::SameLine();
		if (ImGui::Button("10##+10")) delta_v_ += 10.0;
		ImGui::SameLine();
		if (ImGui::Button("100##+100")) delta_v_ += 100.0;
		ImGui::SameLine();
		if (ImGui::Button("1k##+1k")) delta_v_ += 1000.0;


		auto velocity = ship_simu.velocity();
		auto direction = glm::rotate(glm::dvec2(1.0, 0.0), ship_simu.angle());
		velocity += direction * delta_v_;

		auto orbit = compute_orbit_from_state(prim_simu.position(), prim_simu.velocity(), prim_simu.mass(),
		                                       ship_simu.position(), velocity);

		ImGui::LabelText("eccentricity", "%f", orbit.e);

		// draw_orbit(target, tr, prim_simu, ship_simu, sf::Color::Blue);
		auto color = sf::Color::White;
		auto points = screen_orbit_points(prim_simu.position(), orbit, 400, tr);
		std::vector<sf::Vertex> lines;
		for (const auto& p : points) {
			auto t = tr.to_screen(p);
			lines.push_back(to_vertex(t, color));
		}
		target.draw(lines.data(), lines.size(), sf::Lines);

		if (mouse_selecting_anomaly_) {
			mouse_v_ = true_anomaly_from_primary(orbit, prim_simu.position(), tr.to_world(mouse_pos_));
		}

		auto mouse_on_orbit = tr.to_screen(orbit_point(prim_simu.position(), orbit, mouse_v_ + orbit.longitude));

		sf::CircleShape circle(tr.pix_to_res(6));
		circle.setPosition(mouse_on_orbit.x - circle.getRadius(), mouse_on_orbit.y - circle.getRadius());
		target.draw(circle);


		auto ship_v = true_anomaly_from_primary(orbit, prim_simu.position(), ship_simu.position());
		auto dt_to_mouse = compute_dt_from_true_anomaly(orbit, prim_simu.mass(), ship_v, mouse_v_);

		ImGui::LabelText("sel. true anomaly", "%f°", to_degrees(mouse_v_));

		if (dt_to_mouse) {
			ImGui::LabelText("dt to selection", "%s", time_to_string(dt_to_mouse.value()).c_str());

			for(const auto& obj: *object_links_) {
				if (not obj.primary_simu())
					continue;

				auto obj_orbit =
				  compute_orbit_from_state(obj.primary_simu()->position(), obj.primary_simu()->velocity(),
				                           obj.primary_simu()->mass(), obj.simu().position(), obj.simu().velocity());
				auto obj_v =
				  true_anomaly_from_primary(obj_orbit, obj.primary_simu()->position(), obj.simu().position());
				auto obj_predicted_v =
				  compute_true_anomaly_from_dt(obj_orbit, obj.primary_simu()->mass(), obj_v, dt_to_mouse.value());
				auto obj_predicted_pos = tr.to_screen(
				  orbit_point(obj.primary_simu()->position(), obj_orbit, obj_predicted_v + obj_orbit.longitude));

				circle.setPosition(obj_predicted_pos.x - circle.getRadius(), obj_predicted_pos.y - circle.getRadius());
				circle.setFillColor(obj.visu().color());
				target.draw(circle);

			}

		} else {
			ImGui::TextUnformatted("Selected orbit not elliptic");
		}

		if (mouse_selecting_anomaly_) {
			ImGui::Text("Currently selecting");
		} else {
			mouse_selecting_anomaly_ = ImGui::Button("Select true anomaly");
		}

        ImGui::End();
	}

	void set_visible(bool) override {}
	void set_takes_input(bool) override {}

	void handle_mouse_move(const sf::Event::MouseMoveEvent& event) override
	{
		mouse_pos_ = glm::dvec2(event.x, event.y);
	}

	void handle_click(bool pressed, const sf::Event::MouseButtonEvent& event) override
	{
		if (pressed and event.button == sf::Mouse::Button::Left)
			mouse_selecting_anomaly_ = false;
	}

	void set_ship(SimUniverse::Collection::iterator ship)
	{
		ship_ = ship;
		selected_primary_ = std::nullopt;
		selected_hohmann_ = std::nullopt;
	}

private:
	ObjectLinkCollection* object_links_;
	SimUniverse::Collection::iterator ship_;
	std::optional<SimUniverse::Collection::const_iterator> selected_primary_;
	std::optional<SimUniverse::Collection::const_iterator> selected_hohmann_;
	bool mouse_selecting_anomaly_;

	glm::dvec2 mouse_pos_;
	double mouse_v_;

	double delta_v_ = 0.0;
};