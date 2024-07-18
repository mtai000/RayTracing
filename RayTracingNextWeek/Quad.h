#pragma once

#include "rtweekend.h"
#include "Hittable.h"

class Quad : public Hittable {
public:
	Quad(const Point3& q, const Vec3& u, const Vec3& v, shared_ptr<Material> mat)
		: mQuadPoint(q), u(u), v(v), mat(mat)
	{
		auto t_normal = cross(u, v);
		mNormal = normailze(t_normal);
		planeValue = dot(mQuadPoint, mNormal);
		w = t_normal / dot(t_normal, t_normal);
		SetBoundingBox();
	}

	virtual void SetBoundingBox() {
		auto a = aabb(mQuadPoint, mQuadPoint + u + v);
		auto b = aabb(mQuadPoint + u, mQuadPoint + v);
		mBox = aabb(a, b);
	}

	virtual bool isInterior(double a, double b, HitRecord& rec) const {
		Interval inter = Interval(0, 1);

		if (!inter.Contains(a) || !inter.Contains(b))
			return false;

		rec.u = a;
		rec.v = b;
		return true;
	}

	bool Hit(const Ray& r, Interval ray_t, HitRecord& rec) const override {
		//dot(p+td,normal) = a const value ; , there is a plane
		auto  d_normal = dot(mNormal, r.GetDirection());

		if (fabs(d_normal) < Epsilon)
			return false;

		auto t = (planeValue - dot(r.GetOrigin(), mNormal)) / d_normal;
		if (!ray_t.Contains(t))
			return false;

		auto hitPoint = r.At(t);

		Vec3 hitVector = hitPoint - mQuadPoint;
		auto alpha = dot(w, cross(hitVector, v));
		auto beta = dot(w, cross(u, hitVector));

		if (!isInterior(alpha, beta, rec))
			return false;

		rec.t = t;
		rec.p = hitPoint;
		rec.mat = mat;
		rec.SetFaceNormal(r, mNormal);

		return true;
	}

	aabb GetBoundingBox() const override { return mBox; }

private:
	Point3 mQuadPoint;
	Vec3 u;
	Vec3 v;
	Vec3 w;
	shared_ptr<Material> mat;
	aabb mBox;
	Vec3 mNormal;
	double planeValue;
};