#pragma once
#include "rtweekend.h"

#include "Hittable.h"
#include "Material.h"
#include "Texture.h"

//like a light fog or smoke

class ConstantMedium :public Hittable {
public:
	ConstantMedium(shared_ptr<Hittable> boundary, double density, shared_ptr<Texture> tex)
		: mBoundary(boundary), neg_inv_density(-1 / density), mPhaseFunction(make_shared<Isotropic>(tex)) {}

	ConstantMedium(shared_ptr<Hittable> boundary, double density, const Color& albedo)
		: mBoundary(boundary), neg_inv_density(-1 / density), mPhaseFunction(make_shared<Isotropic>(albedo))
	{}

	bool Hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
		HitRecord rec1, rec2;

		if (!mBoundary->Hit(r, Interval::universe, rec1))
			return false;

		if (!mBoundary->Hit(r, Interval(rec1.t + 0.0001, Infinity), rec2))
			return false;

		if (rec1.t < ray_t.min) rec1.t = ray_t.min;
		if (rec2.t > ray_t.max) rec2.t = ray_t.max;

		if (rec1.t >= rec2.t)
			return false;

		if (rec1.t < 0)
			rec1.t = 0;


		auto ray_length = r.GetDirection().length();
		auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;

		auto hit_distance = neg_inv_density * log(random_double());
		if (hit_distance > distance_inside_boundary)
			return false;

		rec.t = rec1.t + hit_distance / ray_length;
		rec.p = r.At(rec.t);

		rec.Normal = Vec3(1, 0, 0);
		rec.front_face = true;
		rec.mat = mPhaseFunction;

		return true;
	}

	aabb GetBoundingBox() const override { return mBoundary->GetBoundingBox(); }

private:
	shared_ptr<Hittable> mBoundary;
	double neg_inv_density;
	shared_ptr<Material> mPhaseFunction;
};