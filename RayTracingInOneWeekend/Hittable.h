#pragma once
#include "rtweekend.h"

class HitRecord {
public:
	Point3 p;
	Vec3 Normal;
	double t;
	bool front_face;
	shared_ptr<class Material> mat;

	void SetFaceNormal(const Ray& r, const Vec3& _normal) {
		front_face = dot(r.GetDirection(), _normal) < 0.0;
		Normal =  front_face ? _normal : -_normal;
	}
};

class Hittable {
public:
	Hittable() {}
	virtual	~Hittable() = default;
	virtual bool Hit(const Ray& ray, Interval inter, HitRecord& hitrecord) const = 0;
};