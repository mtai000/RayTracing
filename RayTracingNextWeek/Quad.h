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

inline shared_ptr<Hittable_list> box(const Point3& a, const Point3& b, shared_ptr<Material> mat)
{
	auto ret = make_shared<Hittable_list>();

	aabb  ab = aabb(a, b);

	auto dx = Vec3(ab.x.Size(), 0, 0);
	auto dy = Vec3(0, ab.y.Size(), 0);
	auto dz = Vec3(0, 0, ab.z.Size());


	// left
	ret->Add(make_shared<Quad>(Point3(ab.x.max, ab.y.min, ab.z.min),  dz, dy, mat));  
	//right
	ret->Add(make_shared<Quad>(Point3(ab.x.min, ab.y.min, ab.z.min),  dy, dz, mat)); 

	//front
	ret->Add(make_shared<Quad>(Point3(ab.x.min, ab.y.min, ab.z.min),  dx, dy, mat));
	//back
	ret->Add(make_shared<Quad>(Point3(ab.x.min, ab.y.min, ab.z.max),  dy, dz, mat)); 

	//top
	ret->Add(make_shared<Quad>(Point3(ab.x.min, ab.y.max, ab.z.min),  dx,  dz, mat));
	//bottom
	ret->Add(make_shared<Quad>(Point3(ab.x.min, ab.y.min, ab.z.min),  dz,  dx, mat)); 

	return ret;
}