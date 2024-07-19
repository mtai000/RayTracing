#pragma once
#include "Hittable.h"
#include "Hittable_list.h"
#include <algorithm>
class bvh : public Hittable {
public:
	bvh(Hittable_list list) : bvh(list.mObjects, 0, list.mObjects.size())
	{
	}

	bvh(std::vector<shared_ptr<Hittable>>& objects, size_t start, size_t end)
	{
		auto size = end - start;
		if (size == 1)
		{
			left = right = objects[start];
			//printf("left and right = %d\n", &objects[start]);
		}
		else if (size == 2)
		{
			left = objects[start];
			right = objects[start + 1];
		}
		else
		{
			mBox = aabb::empty;
			for (auto& obj : objects)
			{
				mBox = aabb(mBox, obj->GetBoundingBox());
			}

			auto axis = mBox.GetLongestAxis();

			std::sort(objects.begin() + start, objects.begin() + end
				, [axis](const shared_ptr<Hittable>& h1, const shared_ptr<Hittable>& h2)
			{
				return h1->GetBoundingBox().GetAxis(axis).min < h2->GetBoundingBox().GetAxis(axis).min;
			});
			size_t mid = start + size / 2;
			left = make_shared<bvh>(objects, start, mid);
			right = make_shared<bvh>(objects, mid, end);
		}
	}

	bool Hit(const Ray& ray, Interval inter, HitRecord& rec) const override
	{
		if (!mBox.Hit(ray, inter))
			return false;
		//virtual bool Hit(const Ray& ray, Interval inter, HitRecord& hitrecord) const = 0;

		auto left_hit = left->Hit(ray, inter, rec);
		auto right_hit = right->Hit(ray, Interval(inter.min, left_hit ? rec.t : inter.max), rec);

		return left_hit || right_hit;
	}

	aabb GetBoundingBox() const override { return mBox; }
	//private:
	shared_ptr<Hittable> left;
	shared_ptr<Hittable> right;
	aabb mBox;

};
