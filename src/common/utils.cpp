#include "utils.h"

#include <vector>
#include <sstream>
#include <iomanip>

#include <glm/detail/func_geometric.hpp>

// ofRectangle getBitmapStringBoundingBox(std::string text){
// 	int maxLineLength = 0;
// 	int lineCount = 0;
// 	std::istringstream iss(text);
//     for(std::string line; std::getline(iss, line); ++lineCount) {
// 		// tabs are not rendered
// 		int currentLineLength = 0;
// 		for(int j = 0; j < (int)line.size(); j++) {
// 			if (line[j] == '\t') {
// 				currentLineLength += 8 - (currentLineLength % 8);
// 			} else {
// 				currentLineLength++;
// 			}
// 		}
// 		maxLineLength = std::max(maxLineLength, currentLineLength);
// 	}

// 	int padding = 4;
// 	int fontSize = 8;
// 	float leading = 1.7;
// 	int height = lineCount * fontSize * leading - 1;
// 	int width = maxLineLength * fontSize;

// 	return ofRectangle(0, 0, width + 2 * padding, height + 2 * padding);
// }

// std::experimental::optional<glm::dvec2>
// point_on_rect(glm::dvec2 position, const ofRectangle& rect)
// {
// 	double minX = rect.getLeft();
// 	double minY = rect.getTop();
// 	double maxX = rect.getRight();
// 	double maxY = rect.getBottom();
// 	double x = position.x;
// 	double y = position.y;

// 	if ((minX <= x && x <= maxX) && (minY <= y && y <= maxY))
// 		return std::experimental::nullopt;

// 	double midX = (minX + maxX) / 2;
// 	double midY = (minY + maxY) / 2;
// 	// if (midX - x == 0) -> m == ±Inf -> minYx/maxYx == x (because value / ±Inf = ±0)
// 	double m = (midY - y) / (midX - x);

// 	if (x <= midX) { // check "left" side
// 		double minXy = m * (minX - x) + y;
// 		if (minY <= minXy && minXy <= maxY)
// 			return glm::dvec2(minX, minXy);
// 	}

// 	if (x >= midX) { // check "right" side
// 		double maxXy = m * (maxX - x) + y;
// 		if (minY <= maxXy && maxXy <= maxY)
// 			return glm::dvec2(maxX, maxXy);
// 	}

// 	if (y <= midY) { // check "top" side
// 		double minYx = (minY - y) / m + x;
// 		if (minX <= minYx && minYx <= maxX)
// 			return glm::dvec2(minYx, minY);
// 	}

// 	if (y >= midY) { // check "bottom" side
// 		double maxYx = (maxY - y) / m + x;
// 		if (minX <= maxYx && maxYx <= maxX)
// 			return glm::dvec2(maxYx, maxY);
// 	}

// 	// edge case when finding midpoint intersection: m = 0/0 = NaN
// 	if (x == midX && y == midY) return glm::dvec2(x, y);

// 	// Should never happen :) If it does, please tell me!
// 	throw std::logic_error("Cannot find intersection " + std::to_string(position.x) + " " + std::to_string(position.y));
// }

// glm::dvec2 clamp_in_rect(glm::dvec2 position, const ofRectangle& rect)
// {
// 	auto ret = point_on_rect(position, rect);
// 	if (ret)
// 		return ret.value();
// 	else
// 		return position;
// }

// bool rect_inside_sphere(const ofRectangle& rect, glm::vec2 position, float radius)
// {
// 	if (glm::length(position - glm::vec2(rect.getTopLeft())) > radius) return false;
// 	if (glm::length(position - glm::vec2(rect.getTopRight())) > radius) return false;
// 	if (glm::length(position - glm::vec2(rect.getBottomLeft())) > radius) return false;
// 	if (glm::length(position - glm::vec2(rect.getBottomRight())) > radius) return false;
// 	return true;
// }

// void align_rect_directional(ofRectangle& rect, const glm::dvec2& position, const glm::dvec2& direction)
// {
// 	rect.alignTo(position);
// 	// double x_dir = direction.x > 0 ? 1.0 : -1.0;
// 	// if (direction.x > 0)
// 	// 	rect.tr
// 	rect.translate(direction * glm::dvec2(rect.getWidth(), rect.getHeight()));
// }

double linear_space(double min, double max, int step_count, int step)
{
	double step_size = (max - min) / (step_count - 1);
	return min + step_size * step;
}

std::string distance_to_string(double distance, int precision)
{
	std::string unit;
	std::stringstream s;

	s << std::defaultfloat;

	if (distance < 1.0) {
		unit = "cm";
		distance *= 100.0;
	} else if (distance < 1.0e3) {
		unit = "m";
	} else if (distance < 1.0e9) {
		unit = "km";
		distance /= 1.0e3;
	// } else if (distance < 1.0e9) {
	// 	unit = "Kkm";
	// 	distance /= 1.0e6;
	} else if (distance < 1.0e12) {
		unit = "Mkm";
		distance /= 1.0e9;
	} else {
		unit = "Bkm";
		distance /= 1.0e12;

		if (distance >= 1.0e6) {
			s << std::scientific << std::setprecision(precision);
		}
	}

	s << distance << " " << unit;
	return s.str();
}

std::string time_to_string(std::uint64_t seconds)
{
	std::stringstream ss;

	std::uint64_t years = seconds / (3600 * 24 * 365);
	seconds %= 3600 * 24 * 365;

	std::uint64_t days = seconds / (3600 * 24);
	seconds %= 3600 * 24;
	std::uint64_t hours = seconds / 3600;
	seconds %= 3600;
	std::uint64_t minutes = seconds / 60;
	seconds %= 60;

	if (years)
		ss << years << "y ";
	if (days)
		ss << days << "d ";

	ss.fill('0');
	ss << std::setw(2) << hours << ":" << std::setw(2) << minutes << ":" << std::setw(2) << seconds;
	return ss.str();
}