#pragma once
#include "rtweekend.h"
#include "Hittable.h"

class Sphere :public Hittable {
public:
	Sphere(const Point3& p, double radius, shared_ptr<class Material> mat)
		: mCenter(p), mRadius(radius > 0 ? radius : 0), mMat(mat), bIsMove(false)
	{
		auto r = Vec3(mRadius, mRadius, mRadius);
		mBox = aabb(mCenter - r, mCenter + r);
	}

	Sphere(const Point3& p1, const Point3& p2, double radius, shared_ptr<class Material> mat)
		: mCenter(p1), mTargetCenter(p2), mRadius(radius > 0 ? radius : 0), mMat(mat), bIsMove(true)
	{
		mMoveDirection = mTargetCenter - mCenter;

		auto r = Vec3(mRadius, mRadius, mRadius);
		auto aabb1 = aabb(mCenter - r, mCenter + r);
		auto aabb2 = aabb(mTargetCenter - r, mTargetCenter + r);
		mBox = aabb(aabb1, aabb2);
	}

	bool Hit(const Ray& r, Interval inter, HitRecord& out_record) const override {
		//printf("Hit sphere: %f\n", mRadius);
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
		GetSphereUV(out_record.Normal, out_record.u, out_record.v);
		out_record.mat = mMat;

		return true;
	}

	void GetSphereUV(const Point3& p, double& u, double& v) const {
		u = (atan2(-p.z(), p.x()) + PI) / (2 * PI);
		v = acos(-p.y()) / PI;
	}

	aabb GetBoundingBox() const override { return mBox; }

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
	aabb mBox;
};