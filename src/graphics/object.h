#pragma once

#include <string>
#include <vector>

#include <glm/vec2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

class GrTransform;

class GrObject {
public:
	GrObject(const std::string& name, const sf::Color& color);
	virtual ~GrObject();

	void update(const glm::dvec2& new_pos, double angle);
	void draw(sf::RenderTarget& target, const GrTransform& tr) const;
	void draw_label(sf::RenderTarget& target, const GrTransform& tr) const;

	const std::string& name() const { return name_; }
	const sf::Color& color() const { return color_; }
	virtual double get_visu_height() const = 0;

protected:
	bool outside_view(const sf::RenderTarget& target, const GrTransform& tr) const;

	virtual void do_draw(sf::RenderTarget& target, const GrTransform& tr) const = 0;
	virtual void do_draw_label(sf::RenderTarget& target, const GrTransform& tr) const;

	virtual double radius() const = 0;

	std::string name_;
	glm::dvec2 position_;
	double angle_;
	sf::Color color_;
};

class GrPlanet: public GrObject {
public:
	GrPlanet(const std::string& name, const sf::Color& color, double radius);
	virtual ~GrPlanet();

	double get_visu_height() const override { return radius_*2; };

protected:
	void do_draw(sf::RenderTarget& target, const GrTransform& tr) const override;
	double radius() const override { return radius_; }
	float radius_;
};

class GrSpaceship: public GrObject {
public:
	GrSpaceship(const sf::Color& color, double width, double height);
	virtual ~GrSpaceship();

	double get_visu_height() const override { return height_; }

	void set_burn(bool burn) { burn_ = burn; }

protected:
	void do_draw(sf::RenderTarget& target, const GrTransform& tr) const override;

	virtual void do_draw_label(sf::RenderTarget& target, const GrTransform& tr) const override;

	double radius() const override { return height_ / 2.0; }

	float width_;
	float height_;
	const std::vector<glm::dvec2> ship_points_;
	const std::vector<glm::dvec2> plume_points_;
	bool burn_;
};