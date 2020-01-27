#include "scene_manager.h"

#include <fstream>

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
  , scene_center_on_(&objects_links_, &camera_, &simu_speed_)
  , scene_ship_orbit_parameters_(&objects_links_)
  , scene_ship_orbit_prediction_(&objects_links_)
  , scenes_({ &scene_universe_, &scene_center_on_, &scene_ship_orbit_parameters_, &scene_ship_orbit_prediction_ })
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
	scene_ship_orbit_parameters_.set_ship(ship_simu_);
	scene_ship_orbit_prediction_.set_ship(ship_simu_);
}

void SceneManager::update(float elapsed)
{
	glm::dvec2 acc(1.0, 0.0);
	acc = glm::rotate(acc, ship_simu_->second.angle());
	acc *= ship_accel_;
	ship_simu_->second.set_inherent_acceleration(acc);

	if (simu_running_) {
		float total_time = elapsed * static_cast<float>(simu_speed_);
		float max_step = 10.0f;
		if (total_time < max_step) {
			simulation_.step(total_time);
		} else {
			float i = 0.0f;
			for (; i < total_time - max_step; i += max_step) {
				simulation_.step(max_step);
			}
			simulation_.step(total_time - i);
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
			case Key::A:
				simu_speed_ *= 2;
				break;
			case Key::Subtract:
			case Key::Z:
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
		mouse_drag_ = {};
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

template<typename T>
void file_write(std::ofstream& file, const T& value)
{
	std::uint32_t size = sizeof(T);
	const char* ptr = reinterpret_cast<const char*>(&value);
	file.write(ptr, size);
}

template<>
void file_write(std::ofstream& file, const std::string& value)
{
	std::uint64_t size = value.size();
	file.write(reinterpret_cast<const char*>(&size), sizeof(size));
	file.write(value.c_str(), size);
}

template<typename T>
T file_read(std::ifstream& file)
{
	T value;
	std::uint32_t size = sizeof(T);
	char* ptr = reinterpret_cast<char*>(&value);
	file.read(ptr, size);
	return value;
}

template<>
std::string file_read(std::ifstream& file)
{
	std::string value;
	std::uint64_t size;
	file.read(reinterpret_cast<char*>(&size), sizeof(size));
	char ch;
	for (std::uint64_t i = 0; i < size; ++i) {
		file.read(&ch, 1);
		value += ch;
	}
	return value;
}

void SceneManager::save(std::string filename)
{
	std::ofstream file(filename, std::ofstream::out | std::ofstream::binary);

	file << "ORBF";

	file_write<std::uint16_t>(file, objects_links_.size());
	for (const auto& obj: objects_links_) {
		file_write(file, obj.name());
		file_write<std::uint32_t>(file, obj.visu().color().toInteger());
		file_write<double>(file, obj.simu().position().x);
		file_write<double>(file, obj.simu().position().y);
		file_write<double>(file, obj.simu().velocity().x);
		file_write<double>(file, obj.simu().velocity().y);
		file_write<double>(file, obj.simu().angular_velocity());
		file_write<double>(file, obj.simu().angle());
		file_write<double>(file, obj.simu().mass());
		file_write<double>(file, obj.simu().radius());
		file_write<char>(file, obj.simu().can_collide());
		file_write<char>(file, (obj.primary_simu() ? true : false));
		if (obj.primary_simu()) {
			file_write(file, obj.primary_name());
		}

		int type = 0;
		if (dynamic_cast<const GrSpaceship*>(&obj.visu()))
			type = 1;
		file_write<char>(file, type);
	}
}

void SceneManager::load(std::string filename)
{
	std::ifstream file(filename, std::ofstream::in | std::ofstream::binary);

	char magic[4];
	file.read(magic, 4);
	if (magic[0] != 'O' || magic[1] != 'R' || magic[2] != 'B' || magic[3] != 'F')
		throw std::runtime_error("Could not load file " + filename + " - not ORBF file.");

	simulation_ = SimUniverse();
	visus_.clear();
	objects_links_.clear();
	ship_visu_ = nullptr;

	auto size = file_read<std::uint16_t>(file);
	for (std::uint16_t i = 0; i < size; ++i) {
		auto name = file_read<std::string>(file);
		auto color = sf::Color(file_read<std::uint32_t>(file));
		glm::dvec2 position(file_read<double>(file), file_read<double>(file));
		glm::dvec2 velocity(file_read<double>(file), file_read<double>(file));
		auto angular_velocity = file_read<double>(file);
		auto angle = file_read<double>(file);
		auto mass = file_read<double>(file);
		auto radius = file_read<double>(file);
		auto can_collide = file_read<char>(file);
		bool has_primary = file_read<char>(file);
		std::string primary_name;
		if (has_primary) {
			primary_name = file_read<std::string>(file);
		}
		auto type = file_read<char>(file);

		simulation_.add_object(name, SimObject(position, velocity, mass, radius, angle, angular_velocity));
		simulation_.find(name)->second.set_can_collide(can_collide);

		std::unique_ptr<GrObject> gr_planet;
		if (type == 0)
			gr_planet = std::make_unique<GrPlanet>(name, color, radius);
		else {
			auto ship = std::make_unique<GrSpaceship>(color, 8, radius * 2);
			ship_visu_ = ship.get();
			gr_planet = std::move(ship);
		}
		std::optional<SimUniverse::Collection::const_iterator> primary = simulation_.objects().find(primary_name);
		if (primary.value() == simulation_.objects().end())
			primary = std::nullopt;

		objects_links_.add({simulation_.objects().find(name), primary, gr_planet.get()});
		visus_.push_back(std::move(gr_planet));
	}

	ship_simu_ = simulation_.find("Ship");
	scene_center_on_.center_camera_on("Ship");
	scene_ship_orbit_parameters_.set_ship(ship_simu_);
	scene_ship_orbit_prediction_.set_ship(ship_simu_);
	ship_accel_ = 0;
	simu_running_ = false;
	simu_speed_ = 1;

}