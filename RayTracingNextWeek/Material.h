#pragma once

#include "rtweekend.h"
#include <cassert>
class Material {
public:
	virtual ~Material() = default;

	virtual bool Scatter(const Ray& in_ray, const HitRecord& rec, Color& attenuation, Ray& scatted)
		const {
		return false;
	}

};


class LamberMaterial :public Material {
public:
	LamberMaterial(const Color& albedo) { mAlbedo = albedo; }

	bool Scatter(const Ray& in_ray, const HitRecord& rec, Color& attenuation, Ray& scatted)
		const override {
		Vec3 direction = rec.Normal + RandomUnitVec();
		if (direction.Near_Zero())
			direction = rec.Normal;

		scatted = Ray(rec.p, direction, in_ray.GetTime());
		attenuation = mAlbedo;
		return true;
	}

private:
	Color mAlbedo;
};



class MetalMaterial : public Material {
public:
	MetalMaterial(const Color& albedo, double fuzz) : mAlbedo(albedo), mFuzz(fuzz > 1 ? 1 : fuzz) {}
	~MetalMaterial() {}
	bool Scatter(const Ray& in_ray, const HitRecord& rec, Color& attenuation, Ray& scatted)
		const override {
		Vec3 reflect_dir = Reflect(in_ray.GetDirection(), rec.Normal);
		reflect_dir = reflect_dir.normalized() + mFuzz * RandomUnitVec();
		scatted = Ray(rec.p, reflect_dir, in_ray.GetTime());
		attenuation = mAlbedo;
		return dot(scatted.GetDirection(), rec.Normal) > 0;
	}
private:
	Color mAlbedo;
	double mFuzz;
};



class DielectricMaterial : public Material {
public:
	DielectricMaterial(const double refract_ratio) : mRefractRatio(refract_ratio) {}
	~DielectricMaterial() {}

	bool Scatter(const Ray& in_ray, const HitRecord& rec, Color& attenuation, Ray& scatted)
		const override
	{
		auto refract_ratio = rec.front_face ? 1 / mRefractRatio : mRefractRatio;

		auto cos_theta = fmin(dot(-in_ray.GetDirection().normalized(), rec.Normal), 1.0);
		auto sin_theta = sqrt(1 - cos_theta * cos_theta);

		Vec3 direction;
		if (refract_ratio * sin_theta > 1.0) {
			direction = Reflect(in_ray.GetDirection().normalized(), rec.Normal);
		}
		else
		{
			direction = Refract(in_ray.GetDirection().normalized(), rec.Normal, refract_ratio);
		}
		scatted = Ray(rec.p, direction, in_ray.GetTime());
		attenuation = Color(1.0, 1.0, 1.0);
		return true;
	}
private:
	double mRefractRatio;

	static double SchlickApproximation(double cos_theta, double ref_ratio) {
		auto r0 = (1 - ref_ratio) / (1 + ref_ratio);
		r0 *= r0;
		return r0 + (1 - r0) * pow((1 - cos_theta), 5);
	}
};