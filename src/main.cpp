#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

// #include <imgui.h>
#include <imgui-SFML.h>

#include <graphics/fonts.h>
#include <scenes/scene_manager.h>

using namespace std;

int main()
{
	sf::ContextSettings settings;
	settings.antialiasingLevel = 4;
	sf::RenderWindow window(sf::VideoMode(1024, 768), "Orbit", sf::Style::Default, settings);
	window.setFramerateLimit(60);

	ImGui::SFML::Init(window, false);
	// ImGui::GetIO().Fonts->
	ImGui::SFML::UpdateFontTexture();

	ImGui::GetStyle().Alpha = 0.8;

	gr_initialize_fonts();

	SceneManager scene_manager(window);

	sf::Clock deltaClock;
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			// Local necessary handling
			ImGui::SFML::ProcessEvent(event);
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::Key::Escape)
				window.close();
			if (event.type == sf::Event::Resized) {
				// Ensure view is not stretched
				auto view = window.getView();
				view.setSize(window.getSize().x, window.getSize().y);
				view.setCenter(window.getSize().x / 2, window.getSize().y / 2);
				window.setView(view);
			}

			// Handle for scenes
			if (not ImGui::GetIO().WantCaptureMouse) {
				if (event.type == sf::Event::MouseWheelScrolled) {
					scene_manager.handle_mouse_wheel(event.mouseWheelScroll);
				}
				if (event.type == sf::Event::MouseMoved) {
					scene_manager.handle_mouse_move(event.mouseMove);
				}
				if (event.type == sf::Event::MouseButtonPressed or event.type == sf::Event::MouseButtonReleased) {
					scene_manager.handle_click(event.type == sf::Event::MouseButtonPressed, event.mouseButton);
				}
			}

			if (not ImGui::GetIO().WantCaptureKeyboard) {
				if (event.type == sf::Event::KeyPressed or event.type == sf::Event::KeyReleased) {
					scene_manager.handle_key(event.type == sf::Event::KeyPressed, event.key);
				}
			}

			if (event.type == sf::Event::Resized) {
				auto size = window.getView().getSize();
				scene_manager.handle_window_resize(size.x, size.y);
			}
		}

		// Update
		ImGui::SFML::Update(window, deltaClock.restart());
		float elapsed = deltaClock.getElapsedTime().asSeconds();
		scene_manager.update(elapsed);

		// Render
		scene_manager.draw(window);
		ImGui::SFML::Render(window); // UI always on top

		window.display();
	}

	return 0;
}
