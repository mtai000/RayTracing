#pragma once
#include "rtweekend.h"

class Interval {
public:
	double min, max;
	Interval() :min(Infinity), max(-Infinity) {}
	Interval(double _min, double _max) :min(_min), max(_max) {}

	double Size() const {
		return max - min;
	}

	bool Contains(double t) const {
		return t <= max && t >= min;
	}

	bool Surround(double t) const {
		return t< max && t > min;
	}

	static const Interval empty, universe;

	double Clamp(double x) const
	{
		if (x < min) return min;
		if (x > max) return max;
		return x;
	}

	Interval Expand(double epsilon)
	{
		epsilon = fabs(epsilon);
		return Interval(min - epsilon, max + epsilon);
	}
};

const Interval Interval::empty = Interval(Infinity, -Infinity);
const Interval Interval::universe = Interval(-Infinity, Infinity);