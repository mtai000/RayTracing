#pragma once
#include "rtweekend.h"
#include "rtw_stb_image.h"
#include "Perlin.h"

class Texture {
public:
	virtual ~Texture() = default;
	virtual Color Value(double u, double v, const Point3& p) const = 0;
};

class SolidColor :public Texture {
public:
	SolidColor(const Color& albedo) : mAlbedo(albedo) {}
	SolidColor(double r, double g, double b) : mAlbedo(Color(r, g, b)) {}
	Color Value(double u, double v, const Point3& p) const override { return mAlbedo; }
private:
	Color mAlbedo;
};

class CheckerTexture :public Texture {
public:
	CheckerTexture(double scale, shared_ptr<Texture> even, shared_ptr<Texture> odd)
		:mScale(scale), mEven(even), mOdd(odd) {}

	CheckerTexture(double scale, const Color& c1, const Color& c2)
		:mScale(scale), mEven(make_shared<SolidColor>(c1)), mOdd(make_shared<SolidColor>(c2)) {}

	Color Value(double u, double v, const Point3& p) const override
	{
		auto x = int(std::floor(p.x() * mScale));
		auto y = int(std::floor(p.y() * mScale));
		auto z = int(std::floor(p.z() * mScale));

		return (x + y + z) % 2 ? mEven->Value(u, v, p) : mOdd->Value(u, v, p);
	}
private:
	double mScale;
	shared_ptr<Texture> mEven;
	shared_ptr<Texture> mOdd;
};

class ImageTexture :public Texture {
public:
	ImageTexture(const char* filename) : mImage(filename) {	}

	Color Value(double u, double v, const Point3& p) const override {
		if (mImage.GetHeight() <= 0) return Color(0, 1, 1);

		u = Interval(0, 1).Clamp(u);
		v = 1.0 - Interval(0, 1).Clamp(v);

		auto i = int(u * mImage.GetWidth());
		auto j = int(v * mImage.GetHeight());

		auto pixel = mImage.pixelData(i, j);
		auto colorScale = 1.0 / 255.0;

		return Color(pixel[0] * colorScale, pixel[1] * colorScale, pixel[2] * colorScale);
	}

private:
	rtw_image mImage;
};


class NoiseTexture : public Texture {
public:
	NoiseTexture() {}

	NoiseTexture(double scale) : mScale(scale) {}

	Color Value(double u, double v, const Point3& p) const override {
		//return Color(1, 1, 1) * (mNoise.SmoothNoise(p * mScale) + 1.0) * 0.5;
		//return Color(1, 1, 1) * mNoise.Turb(p, 7);
		return Color(.5, .5, .5) * (1 + sin(mScale * p.z() + 10 * mNoise.Turb(p, 7)));
	}
private:
	Perlin mNoise;
	double mScale;
};