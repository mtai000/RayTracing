#include <iostream>
#include "rtweekend.h"
#include "Hittable_list.h"
#include "Sphere.h"
#include "Camera.h"
#include "Material.h"
#include "MyTimer.h"
#include "bvh.h"
#include "svh.h"

void Process(const char* output) {
	Hittable_list world;

	auto ground_material = make_shared<LamberMaterial>(Color(0.5, 0.5, 0.5));
	world.Add(make_shared<Sphere>(Point3(0, -1000, 0), 1000, ground_material));

	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			auto _mat = random_double();
			Vec3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

			if ((center - Point3(4, 0.2, 0)).length() > 0.9) {
				shared_ptr<Material> sphere_material;

				if (_mat < 0.8) {
					auto albedo = RandomColor() * RandomColor();
					sphere_material = make_shared<LamberMaterial>(albedo);
					auto center2 = center + Vec3(0, random_double(0, 0.5), 0);
					world.Add(make_shared<Sphere>(center, center2, 0.2, sphere_material));
				}
				else if (_mat < 0.95)
				{
					auto albedo = (RandomUnitVec() + Vec3(1, 1, 1)) * 0.5;
					auto fuzz = random_double(0, 0.5);
					sphere_material = make_shared<MetalMaterial>(albedo, fuzz);
					world.Add(make_shared<Sphere>(center, 0.2, sphere_material));
				}
				else {
					sphere_material = make_shared<DielectricMaterial>(1.5);
					world.Add(make_shared<Sphere>(center, 0.2, sphere_material));
				}
			}
		}
	}

	auto material1 = make_shared <DielectricMaterial >(1.5);
	world.Add(make_shared<Sphere>(Point3(0, 1, 0), 1.0, material1));

	auto material2 = make_shared<LamberMaterial>(Color(0.4, 0.2, 0.1));
	world.Add(make_shared<Sphere>(Point3(-4, 1, 0), 1.0, material2));

	auto material3 = make_shared<MetalMaterial>(Color(0.7, 0.6, 0.5), 0.0);
	world.Add(make_shared<Sphere>(Point3(4, 1, 0), 1.0, material3));

	world = Hittable_list(make_shared<bvh>(world));

	Camera cam;
	cam.SetImageHeightAndAspectRatio(400, 16.0 / 9);
	cam.SetSampleNum(500);
	cam.SetMaxDepth(50);

	cam.SetCameraPara(Vec3(13, 2, 3), Vec3(0, 0, 0), Vec3(0, 1, 0), 25);
	cam.mDefocusAngle = 0.0;
	cam.mDefocusLength = 10.0;

	cam.Render(world);
	cam.WriteBufferToFile(output);
}


int main() {
	MyTimer t;
	Process("image.ppm");
}