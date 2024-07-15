#include <iostream>
#include "rtweekend.h"
#include "Hittable_list.h"
#include "Sphere.h"
#include "Camera.h"
#include "Material.h"
//int main() {
//	Hittable_list world;
//	auto material_ground = make_shared<LamberMaterial>(Color(0.8, 0.8, 0.0));
//	auto material_center = make_shared<LamberMaterial>(Color(0.1, 0.2, 0.5));
//	//auto material_left = make_shared<MetalMaterial>(Color(0.8, 0.8, 0.8), 0.3);
//	auto material_left = make_shared<DielectricMaterial>(1.5);
//	auto material_bubble = make_shared<DielectricMaterial>(1 / 1.5);
//	auto material_right = make_shared<MetalMaterial>(Color(0.8, 0.6, 0.2), 1.0);
//
//	world.Add(make_shared<Sphere>(Point3(0.0, -100.5, -1.0), 100.0, material_ground));
//	world.Add(make_shared<Sphere>(Point3(0.0, 0.0, -1.2), 0.5, material_center));
//	world.Add(make_shared<Sphere>(Point3(-1.0, 0.0, -1.0), 0.5, material_left));
//	world.Add(make_shared<Sphere>(Point3(-1.0, 0.0, -1.0), 0.4, material_bubble));
//	world.Add(make_shared<Sphere>(Point3(1.0, 0.0, -1.0), 0.5, material_right));
//
//
//	Camera cam;
//	cam.SetCameraPara(Vec3(-2, 2, 1), Vec3(0, 0, -1), Vec3(0, 1, 0), 20.0);
//	cam.mDefocusAngle = 10;
//	cam.mDefocusLength = 3.4;
//	cam.SetSampleNum(10);
//	//Need set a bigger number, if set '5' , bubble sphere will be black
//	cam.SetMaxDepth(50);
//	cam.Render(world);
//
//}


int main() {
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
					world.Add(make_shared<Sphere>(center, 0.2, sphere_material));
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

	Camera cam;
	cam.SetImageHeightAndAspectRatio(1800, 16.0 / 9);
	cam.SetSampleNum(500);
	cam.SetMaxDepth(50);

	cam.SetCameraPara(Vec3(13, 2, 3), Vec3(0, 0, 0), Vec3(0, 1, 0), 25);
	cam.mDefocusAngle = 0.6;
	cam.mDefocusLength = 10.0;

	cam.Render(world);
}