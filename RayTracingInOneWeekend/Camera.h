#pragma once
#include "rtweekend.h"
#include "Hittable_list.h"
#include <iomanip>
#include "Material.h"

class Camera {
public:
	Camera() {
		mHeight = 25 * 9;
		mAspectRatio = 16.0 / 9.0;
		mWidth = mHeight * mAspectRatio;
		mSamplePerPixel = 10;
		mMaxDepth = 10;
		mSampleScale = 0.1;
	}
	~Camera() {}

	void Render(Hittable_list& world) {
		Init();
		std::cout << "P3\n" << mWidth << ' ' << mHeight << "\n255\n" << std::endl;

		for (int j = 0; j < mHeight; j++)
		{
			std::clog << '\r' << std::setw(4) << std::setfill('0') << mHeight - j << std::flush;
			for (int i = 0; i < mWidth; i++) {
				Color pixel_color(0.0, 0.0, 0.0);
				for (int k = 0; k < mSamplePerPixel; k++) {
					Ray r = GetRay(i, j);
					pixel_color += RayColor(r, mMaxDepth, world);
				}
				Write_Color(std::cout, pixel_color * mSampleScale);
			}
		}

		std::clog << "\rDone			\n";
	}

	void SetPosition(Vec3 pos) { mPosition = pos; }
	void SetSampleNum(unsigned int _n) { mSamplePerPixel = _n; }
	void SetMaxDepth(unsigned int _d) { mMaxDepth = _d; }
	void SetImageHeightAndAspectRatio(int height, double aspectration)
	{
		mHeight = height;
		mAspectRatio = aspectration;
	}
	void SetCameraPara(const Vec3& pos, const Vec3& lookAt, const Vec3& up, double fov) {
		mPosition = pos;
		mLookFrom = pos;
		mLookAt = lookAt;
		mUp = up;
		mFov = fov;
	}

	Vec3 mLookAt = Vec3(0, 0, -1);
	Vec3 mLookFrom = Vec3(0, 0, 0);;
	Vec3 mUp = Vec3(0, 1, 0);
	double mFov = 90.0;
	double mDefocusAngle = 0;
	double mDefocusLength = 10;
private:
	int mWidth;
	int mHeight;
	unsigned int mSamplePerPixel;
	double mSampleScale;
	unsigned int mMaxDepth;

	double mAspectRatio;
	Vec3 mPosition;

	Vec3 pixel00_loc;
	Vec3 pixel_delta_u;
	Vec3 pixel_delta_v;

	Vec3 defocus_u;
	Vec3 defocus_v;


	Color RayColor(const Ray& r, unsigned int depth, Hittable_list& world) {
		HitRecord hit_rec;
		if (depth <= 0)
			return Color(0.0, 0.0, 0.0);

		if (world.Hit(r, Interval(0.00001, Infinity), hit_rec))
		{
			Ray scatter;
			Color attenuation;
			if (hit_rec.mat->Scatter(r, hit_rec, attenuation, scatter))
			{
				return attenuation * RayColor(scatter, depth - 1, world);
			}
			return Color(0, 0, 0);
		}
		Vec3 _r = normailze(r.GetDirection());
		// t = [0.5,1]
		double a = 0.5 * (_r.y() + 1);
		return (1 - a) * Vec3(1.0, 1.0, 1.0) + a * Vec3(0.5, 0.7, 1.0);
	}

	Ray GetRay(int i, int j) {
		auto offset = Vec3(random_double() - .5, random_double() - .5, 0);
		auto from = mDefocusAngle <= 0 ? mLookFrom : RandomLookFrom();
		return Ray(from, pixel00_loc + (i + offset.x()) * pixel_delta_u + (j + offset.y()) * pixel_delta_v - from);
	}

	void Init()
	{
		mWidth = mHeight * mAspectRatio;
		mSampleScale = 1.0 / mSamplePerPixel;

		// view_height / (lookat - lookfrom).length()  = tan(mFov/2)
		//double viewport_height = tan(degrees_to_radians(mFov / 2)) * (mLookAt - mLookFrom).length() * 2;
		double viewport_height = 2 * tan(degrees_to_radians(mFov / 2)) * mDefocusLength;
		double viewport_width = viewport_height * mAspectRatio;

		Vec3 _forward, _side, _up;

		_forward = (mLookAt - mLookFrom).normalized();
		_side = cross(_forward, mUp);
		_up = cross(_forward, _side);

		Vec3 viewport_u = _side * viewport_width;
		Vec3 viewport_v = _up * viewport_height;

		pixel_delta_u = viewport_u / mWidth;
		pixel_delta_v = viewport_v / mHeight;


		//pixel00_loc = Vec3(-viewport_width / 2, viewport_height / 2, -force_length) + 0.5 * (pixel_delta_u + pixel_delta_v);
		auto viewport_upper_left = mLookFrom + _forward * mDefocusLength  - viewport_u / 2 - viewport_v / 2;
		pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

		auto defocus_raidus = tan(degrees_to_radians(mDefocusAngle / 2)) * mDefocusLength;
		//  pixel_delta_u vs _side
		/*defocus_u = defocus_raidus * pixel_delta_u;
		defocus_v = defocus_raidus * pixel_delta_v;*/

		defocus_u = defocus_raidus * _side;
		defocus_v = defocus_raidus * _up;
	}

	Vec3 RandomLookFrom() {
		auto p = RandomUnitVec();
		return mLookFrom + p[0] * defocus_u + p[1] * defocus_v;
	}
};