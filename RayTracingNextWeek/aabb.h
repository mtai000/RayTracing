#pragma once
#include "rtweekend.h"

class aabb {
public:
	aabb() {}

	aabb(const Interval& _x, const Interval& _y, const Interval& _z)
		: x(_x), y(_y), z(_z)
	{
		PadToMinimums();
	}

	aabb(const Point3& a, const Point3& b) {
		x = a[0] > b[0] ? Interval(b[0], a[0]) : Interval(a[0], b[0]);
		y = a[1] > b[1] ? Interval(b[1], a[1]) : Interval(a[1], b[1]);
		z = a[2] > b[2] ? Interval(b[2], a[2]) : Interval(a[2], b[2]);
		PadToMinimums();
	}

	aabb(const aabb& a, const aabb& b)
	{
		x = Interval(std::fmin(a.x.min, b.x.min), std::fmax(a.x.max, b.x.max));
		y = Interval(std::fmin(a.y.min, b.y.min), std::fmax(a.y.max, b.y.max));
		z = Interval(std::fmin(a.z.min, b.z.min), std::fmax(a.z.max, b.z.max));
		PadToMinimums();
	}

	const Interval& GetAxis(int i) const {
		if (i == 0) return x;
		if (i == 1) return y;
		return z;
	}

	Interval x, y, z;

	double Area()
	{
		return 2 * (x.Size() * y.Size() + y.Size() * z.Size() + z.Size() * x.Size());
	}

	bool Hit(const Ray& r, Interval ray_t) const {
		auto inv_dir = 1 / r.GetDirection();
		for (int i = 0; i < 3; i++) {
			const Interval& a = GetAxis(i);
			auto min = (a.min - r.GetOrigin()[i]) * inv_dir[i];
			auto max = (a.max - r.GetOrigin()[i]) * inv_dir[i];
			if (min > max) std::swap(min, max);
			if (min > ray_t.min) ray_t.min = min;
			if (max < ray_t.max) ray_t.max = max;
			if (ray_t.max <= ray_t.min)
				return false;
		}
		return true;
	}

	unsigned int GetLongestAxis()
	{
		if (x.Size() > y.Size())
			return x.Size() > z.Size() ? 0 : 2;
		else
			return y.Size() > z.Size() ? 1 : 2;
	}

	const static aabb empty, universe;
private:
	void PadToMinimums()
	{
		double epsilon = 0.00001;
		if (x.Size() < epsilon) x = x.Expand(epsilon);
		if (y.Size() < epsilon) y = y.Expand(epsilon);
		if (z.Size() < epsilon) z = z.Expand(epsilon);

	}
};

aabb operator+ (const aabb& box, const Vec3& offset)
{
	return aabb(box.x + offset.x(), box.y + offset.y(), box.z + offset.z());
}

aabb operator+ (const Vec3& offset, const aabb& box)
{
	return aabb(box.x + offset.x(), box.y + offset.y(), box.z + offset.z());
}

const aabb aabb::empty = aabb(Interval::empty, Interval::empty, Interval::empty);
const aabb aabb::universe = aabb(Interval::universe, Interval::universe, Interval::universe);