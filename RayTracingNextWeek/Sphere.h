#pragma once
#include "rtweekend.h"
#include "Hittable.h"

class Sphere :public Hittable {
public:
	Sphere(const Point3& p, double radius, shared_ptr<class Material> mat)
		: mCenter(p), mRadius(radius > 0 ? radius : 0), mMat(mat), bIsMove(false) {}
	Sphere(const Point3& p1, const Point3& p2, double radius, shared_ptr<class Material> mat)
		: mCenter(p1), mTargetCenter(p2), mRadius(radius > 0 ? radius : 0), mMat(mat), bIsMove(true) {
		mMoveDirection = mTargetCenter - mCenter;
	}

	bool Hit(const Ray& r, Interval inter, HitRecord& out_record) const override {

		Point3 curCenter = bIsMove ? GetCurCenter(r.GetTime()) : mCenter;
		auto oc = curCenter - r.GetOrigin();

		auto a = r.GetDirection().length_2();
		auto h = dot(r.GetDirection(), oc);
		auto c = oc.length_2() - mRadius * mRadius;

		auto d = h * h - a * c;

		if (d < 0.0)
			return false;

		auto sqrtd = sqrt(d);
		auto inter_t = (h - sqrtd) / a;

		if (!inter.Contains(inter_t)) {
			inter_t = (h + sqrtd) / a;
			if (!inter.Contains(inter_t)) {
				return false;
			}
		}

		out_record.p = r.At(inter_t);
		out_record.t = inter_t;
		out_record.SetFaceNormal(r, (out_record.p - curCenter) / mRadius);
		out_record.mat = mMat;

		return true;
	}

	Point3 GetCurCenter(double time) const {
		return mCenter + mMoveDirection * time;
	}
private:
	Point3 mCenter;
	Point3 mTargetCenter;
	double mRadius;
	shared_ptr<class Material> mMat;
	Vec3 mMoveDirection;
	bool bIsMove;
};