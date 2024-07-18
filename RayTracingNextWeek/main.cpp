#include <iostream>
#include "rtweekend.h"
#include "Hittable_list.h"
#include "Sphere.h"
#include "Camera.h"
#include "Material.h"
#include "MyTimer.h"
#include "bvh.h"
#include "svh.h"
#include "Color.h"
#include "Quad.h"

std::string output = "image.ppm";

void Process() {
	Hittable_list world;

	auto Checker = make_shared<CheckerTexture>(3, Color(.2, .3, .1), Color(.9, .9, .9));
	auto ground_material = make_shared<LamberMaterial>(Checker);
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
	cam.SetImageHeightAndAspectRatio(600, 16.0 / 9);
	cam.SetSampleNum(50);
	cam.SetMaxDepth(50);

	cam.SetCameraPara(Vec3(13, 2, 3), Vec3(0, 0, 0), Vec3(0, 1, 0), 25);
	cam.mDefocusAngle = 0.0;
	cam.mDefocusLength = 10.0;

	cam.Render(world);
	cam.WriteBufferToFile(output.c_str());
}

void CheckerSpheres() {
	Hittable_list world;

	auto checker = make_shared<CheckerTexture>(3, Color(.2, .3, .1), Color(.9, .9, .9));

	world.Add(make_shared<Sphere>(Point3(0, -10, 0), 10, make_shared<LamberMaterial>(checker)));
	world.Add(make_shared<Sphere>(Point3(0, 10, 0), 10, make_shared<LamberMaterial>(checker)));

	Camera cam;
	cam.SetImageHeightAndAspectRatio(800, 16.0 / 9);
	cam.SetSampleNum(50);
	cam.SetMaxDepth(50);

	cam.SetCameraPara(Vec3(13, 2, 3), Vec3(0, 0, 0), Vec3(0, 1, 0), 25);
	cam.mDefocusAngle = 0.0;
	cam.mDefocusLength = 10.0;

	cam.Render(world);
	cam.WriteBufferToFile(output.c_str());
}

void earth()
{
	auto earth_texture = make_shared<ImageTexture>("image/earthmap.jpg");
	auto earth_surface = make_shared<LamberMaterial>(earth_texture);
	auto globe = make_shared<Sphere>(Point3(0, 0, 0), 2, earth_surface);
	Hittable_list world;
	world.Add(globe);
	Camera cam;
	cam.SetImageHeightAndAspectRatio(800, 16.0 / 9);
	cam.SetSampleNum(50);
	cam.SetMaxDepth(50);

	cam.SetCameraPara(Vec3(13, 2, 3), Vec3(0, 0, 0), Vec3(0, 1, 0), 25);
	cam.mDefocusAngle = 0.0;
	cam.mDefocusLength = 10.0;

	cam.Render(world);
	cam.WriteBufferToFile(output.c_str());
}

void PerlinSpheres() {
	Hittable_list world;
	auto pertext = make_shared<NoiseTexture>(4);
	world.Add(make_shared<Sphere>(Point3(0, -1000, 0), 1000, make_shared<LamberMaterial>(pertext)));
	world.Add(make_shared<Sphere>(Point3(0, 2, 0), 2, make_shared<LamberMaterial>(pertext)));

	Camera cam;
	cam.SetImageHeightAndAspectRatio(800, 16.0 / 9);
	cam.SetSampleNum(50);
	cam.SetMaxDepth(50);

	cam.SetCameraPara(Vec3(13, 2, 3), Vec3(0, 0, 0), Vec3(0, 1, 0), 25);
	cam.mDefocusAngle = 0.0;
	cam.mDefocusLength = 10.0;

	cam.Render(world);
	cam.WriteBufferToFile(output.c_str());
}

void Quads() {
	//auto green = make_shared<LamberMaterial>(Color(0.2, 1.0, 0.2));
	//auto back = make_shared<Quad>(Point3(-2, -2, 0), Vec3(4, 0, 0), Vec3(0, 4, 0), green);
	//Ray test(Vec3(0, 0, 1), Vec3(0, 0, -1), 1.0);
	//HitRecord rec;
	//back->Hit(test, Interval(0, 1.0), rec);
	Hittable_list world;

	auto left_red		= make_shared<LamberMaterial>(Color(1.0, 0.2, 0.2));
	auto back_green		= make_shared<LamberMaterial>(Color(0.2, 1.0, 0.2));
	auto right_blue		= make_shared<LamberMaterial>(Color(0.2, 0.2, 1.0));
	auto upper_orange	= make_shared<LamberMaterial>(Color(1.0, 0.5, 0.0));
	auto lower_teal		= make_shared<LamberMaterial>(Color(0.2, 0.8, 0.8));

	world.Add(make_shared<Quad>(Point3(-3, -2, 5), Vec3(0, 0, -4), Vec3(0, 4,  0), left_red));
	world.Add(make_shared<Quad>(Point3(-2, -2, 0), Vec3(4, 0,  0), Vec3(0, 4,  0), back_green));
	world.Add(make_shared<Quad>(Point3( 3, -2, 1), Vec3(0, 0,  4), Vec3(0, 4,  0), right_blue));
	world.Add(make_shared<Quad>(Point3(-2,  3, 1), Vec3(4, 0,  0), Vec3(0, 0,  4), upper_orange));
	world.Add(make_shared<Quad>(Point3(-2, -3, 5), Vec3(4, 0,  0), Vec3(0, 0, -4), lower_teal));


	Camera cam;
	cam.SetImageHeightAndAspectRatio(800, 1.0);
	cam.SetSampleNum(50);
	cam.SetMaxDepth(50);

	cam.SetCameraPara(Vec3(0, 0, 9), Vec3(0, 0, 0), Vec3(0, 1, 0), 80);
	cam.mDefocusAngle = 0.0;

	cam.Render(world);
	cam.WriteBufferToFile(output.c_str());
}

int main() {
	MyTimer t;
	Quads();
}