#pragma once

#include <cmath>
#include <numeric>
#include <memory>
#include <limits>
#include <random>
#include <iostream>

using std::shared_ptr;
using std::make_shared;

constexpr double Infinity = std::numeric_limits<double>::infinity();
constexpr double PI = 3.1415926535897932385;

inline double degrees_to_radians(double degrees) {
	return degrees * PI / 180;
}

inline double random_double() {
	static std::uniform_real_distribution<double> distribution(0.0, 1.0);
	static std::mt19937 generator;
	return distribution(generator);
}

inline double random_double(double min, double max)
{
	return random_double() * (max - min) + min;
}

template<typename T>
inline T clamp(T x, T low, T high)
{
	return x < low ? low : (x > high ? high : x);
}

#include "Vec3.h"
#include "Ray.h"
#include "Interval.h"
#include "Color.h"
#include "Hittable.h"
#include "Ray.h"
#include "Material.h"