#pragma once
#include "Hittable.h"
#include "Hittable_list.h"
class svh : public Hittable {
public:
	svh(Hittable_list list) : svh(list.mObjects, 0, list.mObjects.size())
	{
	}

	svh(std::vector<shared_ptr<Hittable>>& objects, size_t start, size_t end)
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

			//std::cout << start << '\t' << end << std::endl;
			std::sort(objects.begin() + start, objects.begin() + end
				, [axis](const shared_ptr<Hittable>& h1, const shared_ptr<Hittable>& h2)
			{
				return h1->GetBoundingBox().GetAxis(axis).min < h2->GetBoundingBox().GetAxis(axis).min;
			});


			double cost = 15.0;
			aabb a = aabb::empty;
			aabb b = aabb::empty;
			size_t split = 0;

			for (size_t i = start; i < end; i++)
			{
				for (size_t j = start; j < i; j++)
				{
					a = aabb(a, objects[j]->GetBoundingBox());
				}
				for (size_t j = i + 1; j < end; j++)
				{
					b = aabb(b, objects[j]->GetBoundingBox());
				}

				double t_cost = ((i + 1) * a.Area()
					+ (end - i) * b.Area()) / mBox.Area();

				if (i == 0) cost = t_cost;
				if (t_cost < cost)
				{
					split = i;
					cost = t_cost;
				}
			}

			//size_t mid = start + size / 2;
			left = make_shared<svh>(objects, start, split);
			right = make_shared<svh>(objects, split, end);
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
private:
	shared_ptr<Hittable> left;
	shared_ptr<Hittable> right;
	aabb mBox;
};