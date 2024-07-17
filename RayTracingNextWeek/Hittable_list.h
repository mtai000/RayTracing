#pragma once
#include "rtweekend.h"
#include "Hittable.h"

class Hittable_list :public Hittable {
public:
	std::vector<std::shared_ptr<Hittable>> mObjects;
	Hittable_list() {}
	Hittable_list(std::shared_ptr<Hittable> _obj) { Add(_obj); }
	void Add(std::shared_ptr<Hittable> _obj)
	{
		mObjects.push_back(_obj);
		mBox = aabb(mBox, _obj->GetBoundingBox());
	}

	bool Hit(const Ray& ray, Interval inter, HitRecord& rec) const override {
		HitRecord tmp_rec;
		bool hitAnything = false;
		double t_nearest = inter.max;

		for (auto& obj : mObjects) 
		{
			if (obj->Hit(ray, Interval(inter.min, t_nearest), tmp_rec)) 
			{
				hitAnything = true;
				t_nearest = tmp_rec.t;
				rec = tmp_rec;
			}
		}

		return hitAnything;
	}

	aabb GetBoundingBox() const override { return mBox; }

private:
	aabb mBox;

};