#pragma once
#include "rtweekend.h"
#include "Hittable_list.h"
#include <iomanip>
#include "Material.h"
#include <fstream>
#include <thread>
#include <mutex>
#include <string>
#include "stb_image_write.h"

class Camera {
public:
	Camera() {
		mHeight = 25 * 9;
		mAspectRatio = 16.0 / 9.0;
		mWidth = mHeight * mAspectRatio;
		mSamplePerPixel = 10;
		mMaxDepth = 10;
		mSampleScale = 0.1;
		mBackgroundColor = Color(0.70, 0.80, 1.00);
	}
	~Camera() {}


	static void RenderThread(Hittable_list& world, int startHeight, int endHeight, Camera& cam)
	{
		for (int h = startHeight; h < endHeight; h++)
		{
			for (int w = 0; w < cam.mWidth; w++) {
				Color pixel_color(0.0, 0.0, 0.0);
				for (int k = 0; k < cam.mSamplePerPixel; k++) {
					Ray r = cam.GetRay(w, h);
					pixel_color += cam.RayColor(r, cam.mMaxDepth, world);
				}
				cam.mColorFrame[h * cam.mWidth + w] = pixel_color * cam.mSampleScale;
			}

			cam.g_mutex.lock();
			cam.complete_line++;
			std::clog << std::setw(4) << std::setfill(' ') << cam.mHeight - cam.complete_line << '\r' << std::flush;
			cam.g_mutex.unlock();
		}
	}

	void Render(Hittable_list& world, bool bUseMultiThread = true) {
		Init();
		//std::cout << "P3\n" << mWidth << ' ' << mHeight << "\n255\n" << std::endl;

		auto core_num = std::thread::hardware_concurrency();
		std::cout << "Core number is : " << core_num << std::endl;
		//core_num = 1;
		auto heightForThread = mHeight / core_num;
		std::vector<std::thread> jobs;

		if (bUseMultiThread)
		{
			for (int i = 0; i < core_num; i++) {
				jobs.push_back(std::thread(RenderThread, std::ref(world), i * heightForThread, std::fmin((i + 1) * heightForThread, mHeight), std::ref(*this)));
			}

			for (int i = 0; i < jobs.size(); i++)
				jobs[i].join();
		}
		else
		{
			RenderThread(world, 0, mHeight, *this);
		}

		std::clog << "\rDone			\n";
	}


	void WriteBufferToFile(const char* filename)
	{
		std::ofstream file;
		file.open(filename);
		file << "P3\n" << mWidth << ' ' << mHeight << "\n255\n" << std::endl;
		std::string outStr = "";
		std::vector<char> outputBuffer;
		for (int i = 0; i < mColorFrame.size(); i++)
		{
			auto r = mColorFrame[i].x();
			auto g = mColorFrame[i].y();
			auto b = mColorFrame[i].z();
			r = linear_to_gamma(r);
			g = linear_to_gamma(g);
			b = linear_to_gamma(b);

			static const Interval interval(0.0, 0.999);
			auto ir = int(256 * interval.Clamp(r));
			auto ig = int(256 * interval.Clamp(g));
			auto ib = int(256 * interval.Clamp(b));

			outStr += std::to_string(ir) + ' ' + std::to_string(ig) + ' ' + std::to_string(ib) + '\n';
			//file << ir << ' ' << ig << ' ' << ib << std::endl;
			//Write_Color(file, mColorFrame[i]);
			outputBuffer.push_back(ir);
			outputBuffer.push_back(ig);
			outputBuffer.push_back(ib);
		}
		file << outStr;
		file.close();

		std::string pngFile = std::string(filename) + ".png";
		std::cout << "Output png file : " << pngFile << std::endl;
		stbi_write_png(pngFile.c_str(), mWidth, mHeight, 3, mColorFrame.data(), 0);
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

	void SetBackground(const Color& c)
	{
		mBackgroundColor = c;
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

	std::mutex g_mutex;
	int complete_line = 0;
	Color mBackgroundColor;

	Vec3 defocus_u;
	Vec3 defocus_v;
	std::vector<Color> mColorFrame;

	Color RayColor(const Ray& r, unsigned int depth, Hittable_list& world) {
		HitRecord hit_rec;
		if (depth <= 0)
			return Color(0.0, 0.0, 0.0);

		if (!world.Hit(r, Interval(0.00001, Infinity), hit_rec))
		{
			return mBackgroundColor;
		}

		Ray scatter;
		Color attenuation;
		Color colorFromEmission = hit_rec.mat->Emitted(hit_rec.u, hit_rec.v, hit_rec.p);

		if (!hit_rec.mat->Scatter(r, hit_rec, attenuation, scatter))
		{
			return colorFromEmission;
		}

		auto ColorFromScatter = attenuation * RayColor(scatter, depth - 1, world);

		return colorFromEmission + ColorFromScatter;
	}

	Ray GetRay(int i, int j) {
		auto offset = Vec3(random_double() - .5, random_double() - .5, 0);
		auto from = mDefocusAngle <= 0 ? mLookFrom : RandomLookFrom();
		return Ray(from, pixel00_loc + (i + offset.x()) * pixel_delta_u + (j + offset.y()) * pixel_delta_v - from, random_double());
	}

	void Init()
	{
		mWidth = mHeight * mAspectRatio;
		mSampleScale = 1.0 / mSamplePerPixel;
		mColorFrame.resize(mWidth * mHeight);
		std::fill(std::begin(mColorFrame), std::end(mColorFrame), Color(0.0, 0.0, 0.0));
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
		auto viewport_upper_left = mLookFrom + _forward * mDefocusLength - viewport_u / 2 - viewport_v / 2;
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
