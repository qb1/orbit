#include "scene_manager.h"

#include <glm/gtx/rotate_vector.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include <simulation/object.h>
#include <simulation/universe.h>

#include <graphics/camera.h>
#include <graphics/misc_draw.h>
#include <graphics/object.h>
#include <graphics/fonts.h>

#include <common/universe_definitions.h>
#include <common/utils.h>

SceneManager::~SceneManager() = default;

SceneManager::SceneManager(const sf::RenderTarget& target)
  : scene_universe_(&objects_links_)
  , scene_center_on_(&objects_links_, &camera_)
  , scenes_({ &scene_universe_, &scene_center_on_ })
  , camera_(target.getSize().x, target.getSize().y)
  , ship_accel_(0.0)
  , simu_running_(false)
  , simu_speed_(1)
{
	init_simulation_randomize(simulation_, solar_system_definition);

	for (const auto& obj: solar_system_definition) {
		auto gr_planet = std::make_unique<GrPlanet>(obj.name, obj.color, obj.radius);
		std::optional<SimUniverse::Collection::const_iterator> primary = simulation_.objects().find(obj.primary_name);
		if (primary.value() == simulation_.objects().end())
			primary = std::nullopt;

		objects_links_.add({simulation_.objects().find(obj.name), primary, gr_planet.get()});
		visus_.push_back(std::move(gr_planet));
	}

	const auto& earth = objects_links_.find("Earth").simu();

	double ship_radius = 10.0;
	double ship_angle = 0.0;
	simulation_.add_object(
	  "Ship", SimObject(earth.position() + glm::dvec2(earth.radius() + ship_radius + 200, 0.0),
	                    earth.velocity(), 30.0e3, ship_radius, ship_angle, 0.0));
	ship_simu_ = simulation_.find("Ship");
	ship_simu_->second.set_can_collide(true);

	auto ship_visu =std::make_unique<GrSpaceship>(sf::Color::Red, 8, ship_radius * 2);
	ship_visu_ = ship_visu.get();
	objects_links_.add({ship_simu_, std::nullopt, ship_visu_});
	visus_.push_back(std::move(ship_visu));

	scene_center_on_.center_camera_on("Ship");
}

void SceneManager::update(float elapsed)
{
	glm::dvec2 acc(1.0, 0.0);
	acc = glm::rotate(acc, ship_simu_->second.angle());
	acc *= ship_accel_;
	ship_simu_->second.set_inherent_acceleration(acc);

	if (simu_running_) {
		int speed = std::min(simu_speed_, 600);
		for (int i=0; i < simu_speed_; i += speed) {
			simulation_.step(1.0 / 60.0 * speed);
		}
		for (int i=0; i < simu_speed_ % 600; i++) {
			simulation_.step(1.0 / 60.0);
		}
	}

	for (auto& link: objects_links_) {
		link.visu().update(link.simu().position(), link.simu().angle());
	}

	for (auto scene : scenes_) {
		scene->update(elapsed);
	}
}

void SceneManager::draw(sf::RenderTarget& target)
{
	target.clear(sf::Color(15, 15, 15));

    auto tr = camera_.transform();

    draw_grid(target, tr, camera_.origin);

    for (auto scene : scenes_) {
		scene->draw(target, tr);
	}

	std::string status_text = "Ship accel: " + distance_to_string(ship_accel_, 0) + "/s - "
						      "Elapsed time: " +time_to_string(simulation_.date()) + " - "
						      "Simu x" + std::to_string(simu_speed_);

	if (not simu_running_) {
		status_text += " (Paused)";
	}
	sf::Text status_bar(status_text, GrFonts().info, tr.height_to_res(13));
	status_bar.setPosition(0, target.getSize().y - status_bar.getLocalBounds().height);
	sf::RectangleShape rect(sf::Vector2f(target.getSize().x, status_bar.getLocalBounds().height));
	rect.setPosition(status_bar.getPosition());
	rect.setFillColor(sf::Color(0x00000080));
	target.draw(rect);
	target.draw(status_bar);
}

void SceneManager::handle_window_resize(int w, int h)
{
	camera_.update_window(w, h);

	for (auto scene : scenes_) {
		scene->handle_window_resize(w, h);
	}
}

void SceneManager::handle_key(bool pressed, const sf::Event::KeyEvent& event)
{
	const double accel_step = 1.0;
	const double rotation_step = glm::two_pi<double>() / 360.0 * 5.0;

	if (pressed) {
		using Key = sf::Keyboard::Key;
		switch(event.code) {
			case Key::Space:
				simu_running_ = !simu_running_;
				break;
			case Key::Add:
				simu_speed_ *= 2;
				break;
			case Key::Subtract:
				simu_speed_ /= 2;
				break;
			case Key::Up:
				ship_accel_ += accel_step;
				break;
			case Key::Down:
				ship_accel_ -= accel_step;
				break;
			case Key::Left:
				ship_simu_->second.set_angle(ship_simu_->second.angle() - rotation_step);
				ship_simu_->second.set_angular_velocity(0.0);
				break;
			case Key::Right:
				ship_simu_->second.set_angle(ship_simu_->second.angle() + rotation_step);
				ship_simu_->second.set_angular_velocity(0.0);
				break;
			default:
				break;
		}
		simu_speed_ = std::min(simu_speed_, 1024 * 1024);
		simu_speed_ = std::max(simu_speed_, 1);

		ship_accel_ = std::min(ship_accel_, 30.0);
		ship_accel_ = std::max(0.0, ship_accel_);
		ship_visu_->set_burn(ship_accel_ > 0);
	}

	for (auto scene : scenes_) {
		scene->handle_key(pressed, event);
	}
}

void SceneManager::handle_click(bool pressed, const sf::Event::MouseButtonEvent& event)
{
	if (event.button != sf::Mouse::Button::Left)
		return;

	if (pressed) {
		mouse_drag_ = glm::dvec2(event.x, event.y);
	} else {
		mouse_drag_.reset();
	}

	for (auto scene : scenes_) {
		scene->handle_click(pressed, event);
	}
}

void SceneManager::handle_mouse_move(const sf::Event::MouseMoveEvent& event)
{
	if (mouse_drag_) {
		glm::dvec2 new_vec(event.x, event.y);
		camera_.translate_screen(new_vec - mouse_drag_.value());
		mouse_drag_ = new_vec;
	}

	for (auto scene : scenes_) {
		scene->handle_mouse_move(event);
	}
}

void SceneManager::handle_mouse_wheel(const sf::Event::MouseWheelScrollEvent& event)
{
	camera_.scale += camera_.scale * 0.1 * event.delta;

	for (auto scene : scenes_) {
		scene->handle_mouse_wheel(event);
	}
}
