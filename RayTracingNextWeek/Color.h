#pragma once

#include "Interval.h"
typedef Vec3 Color;

inline double linear_to_gamma(double val)
{
	return val > 0 ? sqrt(val) : 0;
}

void Write_Color(std::ostream& outStream, const Color& pixel_color) {
	auto r = pixel_color.x();
	auto g = pixel_color.y();
	auto b = pixel_color.z();

	r = linear_to_gamma(r);
	g = linear_to_gamma(g);
	b = linear_to_gamma(b);

	static const Interval interval(0.0, 0.999);
	auto ir = int(256 * interval.Clamp(r));
	auto ig = int(256 * interval.Clamp(g));
	auto ib = int(256 * interval.Clamp(b));

	std::cout << ir << ' ' << ig << ' ' << ib << std::endl;
}

Color RandomColor() {
	return Color(random_double(), random_double(), random_double());
}

inline Color operator* (const Color& attenuation, const Color& color) {
	return Color(attenuation.x() * color.x(), attenuation.y() * color.y(), attenuation.z() * color.z());
}

