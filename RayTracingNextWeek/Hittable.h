#pragma once
#include "rtweekend.h"
#include "aabb.h"

class HitRecord {
public:
	Point3 p;
	Vec3 Normal;
	double t;
	double u;
	double v;
	bool front_face;

	shared_ptr<class Material> mat;

	void SetFaceNormal(const Ray& r, const Vec3& _normal) {
		front_face = dot(r.GetDirection(), _normal) < 0.0;
		Normal = front_face ? _normal : -_normal;
	}
};

class Hittable {
public:
	virtual	~Hittable() = default;
	virtual bool Hit(const Ray& ray, Interval inter, HitRecord& hitrecord) const = 0;
	virtual aabb GetBoundingBox() const = 0;
};

class Translate :public Hittable {
public:
	Translate(shared_ptr<Hittable> obj, const Vec3& off)
		:object(obj), offset(off)
	{
		mBox = object->GetBoundingBox() + offset;
	}

	bool Hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
		Ray offset_r(r.GetOrigin() - offset, r.GetDirection(), r.GetTime());
		if (!object->Hit(offset_r, ray_t, rec))
			return false;

		rec.p += offset;
		return true;
	}

	aabb GetBoundingBox() const override { return mBox; }

private:
	shared_ptr<Hittable> object;
	Vec3 offset;
	aabb mBox;
};

class RotateY : public Hittable {
public:
	RotateY(shared_ptr<Hittable> object, double angle) : mObject(object) {
		auto radians = degrees_to_radians(angle);
		sin_theta = sin(radians);
		cos_theta = cos(radians);
		mBox = mObject->GetBoundingBox();

		Point3 min(Infinity, Infinity, Infinity);
		Point3 max(-Infinity, -Infinity, -Infinity);

		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				for (int k = 0; k < 2; k++) {
					auto x = i * mBox.x.max + (1 - i) * mBox.x.min;
					auto y = j * mBox.y.max + (1 - j) * mBox.y.min;
					auto z = k * mBox.z.max + (1 - k) * mBox.z.min;

					auto newx = cos_theta * x + sin_theta * z;
					auto newz = -sin_theta * x + cos_theta * z;

					Vec3 tester(newx, y, newz);

					for (int c = 0; c < 3; c++) {
						min[c] = fmin(min[c], tester[c]);
						max[c] = fmax(max[c], tester[c]);
					}
				}
			}
		}

		mBox = aabb(min, max);
	}


	bool Hit(const Ray& r, Interval ray_t, HitRecord& rec) const override
	{
		auto origin = r.GetOrigin();
		auto direction = r.GetDirection();

		origin[0] = cos_theta * r.GetOrigin()[0] - sin_theta * r.GetOrigin()[2];
		origin[2] = sin_theta * r.GetOrigin()[0] + cos_theta * r.GetOrigin()[2];

		direction[0] = cos_theta * r.GetDirection()[0] - sin_theta * r.GetDirection()[2];
		direction[2] = sin_theta * r.GetDirection()[0] + cos_theta * r.GetDirection()[2];

		Ray rotated_r(origin, direction, r.GetTime());

		// Determine whether an intersection exists in object space (and if so, where)
		if (!mObject->Hit(rotated_r, ray_t, rec))
			return false;

		// Change the intersection point from object space to world space
		auto p = rec.p;
		p[0] = cos_theta * rec.p[0] + sin_theta * rec.p[2];
		p[2] = -sin_theta * rec.p[0] + cos_theta * rec.p[2];

		// Change the normal from object space to world space
		auto normal = rec.Normal;
		normal[0] = cos_theta * rec.Normal[0] + sin_theta * rec.Normal[2];
		normal[2] = -sin_theta * rec.Normal[0] + cos_theta * rec.Normal[2];

		rec.p = p;
		rec.Normal = normal;

		return true;
	}

	aabb GetBoundingBox() const override { return mBox; }
private:
	double sin_theta;
	double cos_theta;
	shared_ptr<Hittable> mObject;
	aabb mBox;
};