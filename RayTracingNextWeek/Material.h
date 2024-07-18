#pragma once

#include "rtweekend.h"
#include <cassert>
#include "Texture.h"

class Material {
public:
	virtual ~Material() = default;

	virtual bool Scatter(const Ray& in_ray, const HitRecord& rec, Color& attenuation, Ray& scatted)
		const {
		return false;
	}

	virtual Color Emitted(double u, double v, const Point3& p) const {
		return Color(0, 0, 0);
	}

};


class LamberMaterial :public Material {
public:
	//LamberMaterial(const Color& albedo) { mAlbedo = albedo; }
	LamberMaterial(const Color& c) : mTexture(make_shared<SolidColor>(c)) {}
	LamberMaterial(shared_ptr<Texture> tex) : mTexture(tex) {}
	bool Scatter(const Ray& in_ray, const HitRecord& rec, Color& attenuation, Ray& scatted)
		const override {
		Vec3 direction = rec.Normal + RandomUnitVec();
		if (direction.Near_Zero())
			direction = rec.Normal;

		scatted = Ray(rec.p, direction, in_ray.GetTime());
		attenuation = mTexture->Value(rec.u, rec.v, rec.p);
		return true;
	}

private:
	Color mAlbedo;
	shared_ptr<Texture> mTexture;
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


class DiffuseLight : public Material {
public:
	DiffuseLight(shared_ptr<Texture> tex) :mTexture(tex) {}
	DiffuseLight(const Color& emit) :mTexture(make_shared<SolidColor>(emit)) {}

	Color Emitted(double u, double v, const Point3& p) const override {
		return mTexture->Value(u, v, p);
	}

private:
	shared_ptr<Texture> mTexture;
};

//各向同性
class Isotropic : public Material {
public:
	Isotropic(const Color& albedo) : mTexture(make_shared<SolidColor>(albedo)) {}
	Isotropic(shared_ptr<Texture> tex) : mTexture(tex) {}

	bool Scatter(const Ray& r_in, const HitRecord& rec, Color& attenuation, Ray& scattered)
		const override
	{
		scattered = Ray(rec.p, RandomUnitVec(), r_in.GetTime());
		attenuation = mTexture->Value(rec.u, rec.v, rec.p);
		return true;
	}

private:
	shared_ptr<Texture> mTexture;
};