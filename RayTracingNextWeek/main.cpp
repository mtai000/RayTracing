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
#include "ConstantMedium.h"

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

	auto left_red = make_shared<LamberMaterial>(Color(1.0, 0.2, 0.2));
	auto back_green = make_shared<LamberMaterial>(Color(0.2, 1.0, 0.2));
	auto right_blue = make_shared<LamberMaterial>(Color(0.2, 0.2, 1.0));
	auto upper_orange = make_shared<LamberMaterial>(Color(1.0, 0.5, 0.0));
	auto lower_teal = make_shared<LamberMaterial>(Color(0.2, 0.8, 0.8));

	world.Add(make_shared<Quad>(Point3(-3, -2, 5), Vec3(0, 0, -4), Vec3(0, 4, 0), left_red));
	world.Add(make_shared<Quad>(Point3(-2, -2, 0), Vec3(4, 0, 0), Vec3(0, 4, 0), back_green));
	world.Add(make_shared<Quad>(Point3(3, -2, 1), Vec3(0, 0, 4), Vec3(0, 4, 0), right_blue));
	world.Add(make_shared<Quad>(Point3(-2, 3, 1), Vec3(4, 0, 0), Vec3(0, 0, 4), upper_orange));
	world.Add(make_shared<Quad>(Point3(-2, -3, 5), Vec3(4, 0, 0), Vec3(0, 0, -4), lower_teal));


	Camera cam;
	cam.SetImageHeightAndAspectRatio(800, 1.0);
	cam.SetSampleNum(50);
	cam.SetMaxDepth(50);

	cam.SetCameraPara(Vec3(0, 0, 9), Vec3(0, 0, 0), Vec3(0, 1, 0), 80);
	cam.mDefocusAngle = 0.0;

	cam.Render(world);
	cam.WriteBufferToFile(output.c_str());
}

void CornelBox()
{
	Hittable_list world;
	auto red = make_shared<LamberMaterial>(Color(.65, .05, .05));
	auto white = make_shared<LamberMaterial>(Color(.93, .93, .93));
	auto green = make_shared<LamberMaterial>(Color(.12, .45, .15));
	auto grey = make_shared<LamberMaterial>(Color(.3, .3, .3));

	auto light = make_shared<DiffuseLight>(Color(15, 15, 15));

	world.Add(make_shared<Quad>(Point3(555, 0, 0), Vec3(0, 555, 0), Vec3(0, 0, 555), green));
	world.Add(make_shared<Quad>(Point3(0, 0, 0), Vec3(0, 555, 0), Vec3(0, 0, 555), red));
	world.Add(make_shared<Quad>(Point3(343, 554, 332), Vec3(-130, 0, 0), Vec3(0, 0, -105), light));
	world.Add(make_shared<Quad>(Point3(0, 0, 0), Vec3(555, 0, 0), Vec3(0, 0, 555), white));
	world.Add(make_shared<Quad>(Point3(555, 555, 555), Vec3(-555, 0, 0), Vec3(0, 0, -555), white));
	world.Add(make_shared<Quad>(Point3(0, 0, 555), Vec3(555, 0, 0), Vec3(0, 555, 0), white));


	shared_ptr<Hittable> box1 = box(Point3(0, 0, 0), Point3(165, 230, 165), white);
	box1 = make_shared<RotateY>(box1, 15);
	box1 = make_shared<Translate>(box1, Vec3(265, 0, 295));
	world.Add(box1);

	shared_ptr<Hittable> box2 = box(Point3(0, 0, 0), Point3(165, 165, 165), grey);
	box2 = make_shared<RotateY>(box2, -18);
	box2 = make_shared<Translate>(box2, Vec3(130, 0, 65));
	world.Add(box2);

	Camera cam;
	cam.SetImageHeightAndAspectRatio(400, 1.0);
	cam.SetSampleNum(200);
	cam.SetMaxDepth(50);
	cam.SetBackground(Color(0, 0, 0));

	cam.SetCameraPara(Vec3(278, 278, -800), Vec3(278, 278, 0), Vec3(0, 1, 0), 40);
	cam.mDefocusAngle = 0.0;

	cam.Render(world);
	cam.WriteBufferToFile(output.c_str());
}

void CornellSmoke() {
	Hittable_list world;
	auto red = make_shared<LamberMaterial>(Color(.65, .05, .05));
	auto white = make_shared<LamberMaterial>(Color(.73, .73, .73));
	auto green = make_shared<LamberMaterial>(Color(.12, .45, .15));
	auto light = make_shared<DiffuseLight>(Color(7, 7, 7));


	world.Add(make_shared<Quad>(Point3(555, 0, 0), Vec3(0, 555, 0), Vec3(0, 0, 555), green));
	world.Add(make_shared<Quad>(Point3(0, 0, 0), Vec3(0, 555, 0), Vec3(0, 0, 555), red));
	world.Add(make_shared<Quad>(Point3(113, 554, 127), Vec3(330, 0, 0), Vec3(0, 0, 305), light));
	//world.Add(make_shared<Quad>(Point3(343, 554, 332), Vec3(-130, 0, 0), Vec3(0, 0, -105), light));

	world.Add(make_shared<Quad>(Point3(0, 555, 0), Vec3(555, 0, 0), Vec3(0, 0, 555), white));
	world.Add(make_shared<Quad>(Point3(0, 0, 0), Vec3(555, 0, 0), Vec3(0, 0, 555), white));
	world.Add(make_shared<Quad>(Point3(0, 0, 555), Vec3(555, 0, 0), Vec3(0, 555, 0), white));

	shared_ptr<Hittable> box1 = box(Point3(0, 0, 0), Point3(165, 230, 165), white);
	box1 = make_shared<RotateY>(box1, 18);
	box1 = make_shared<Translate>(box1, Vec3(265, 0, 295));

	shared_ptr<Hittable> box2 = box(Point3(0, 0, 0), Point3(165, 165, 165), white);
	box2 = make_shared<RotateY>(box2, -15);
	box2 = make_shared<Translate>(box2, Vec3(130, 0, 65));

	world.Add(make_shared<ConstantMedium>(box1, 0.01, Color(0, 0, 0)));
	world.Add(make_shared<ConstantMedium>(box2, 0.01, Color(0, 0.5, 1)));
	//world.Add(box1);
	//world.Add(box2);

	Camera cam;
	cam.SetImageHeightAndAspectRatio(1200, 1.0);
	cam.SetSampleNum(500);
	cam.SetMaxDepth(50);
	cam.SetBackground(Color(0, 0, 0));

	cam.SetCameraPara(Vec3(278, 278, -800), Vec3(278, 278, 0), Vec3(0, 1, 0), 40);
	cam.mDefocusAngle = 0.0;

	cam.Render(world);
	cam.WriteBufferToFile(output.c_str());
}


void FinalScene(int height, int samplesPerPixel, int max_depth)
{

	//ground boxes
	Hittable_list boxes;
	auto ground = make_shared<LamberMaterial>(Color(.48, .83, .53));

	int boxesPerSide = 20;
	for (int i = 0; i < boxesPerSide; i++)
	{
		for (int j = 0; j < boxesPerSide; j++)
		{
			auto w = 100.0;
			auto x0 = -1000.0 + j * w;
			auto z0 = -1000.0 + j * w;

			auto y0 = 0.0;
			auto x1 = x0 + w;
			auto y1 = random_double(1, 101);
			auto z1 = z0 + w;

			boxes.Add(box(Point3(x0, y0, z0), Point3(x1, y1, z1), ground));
		}
	}

	// world add ground
	Hittable_list world;
	world.Add(make_shared<bvh>(boxes));

	// add light
	auto light = make_shared<DiffuseLight>(Color(7, 7, 7));
	world.Add(make_shared<Quad>(Point3(123, 544, 147), Vec3(300, 0, 0), Vec3(0, 0, 265), light));

	//add moving  sphere
	auto center1 = Point3(400, 400, 200);
	auto center2 = center1 + Vec3(30, 0, 0);
	auto sphereMaterial = make_shared<LamberMaterial>(Color(.7, .3, .1));
	world.Add(make_shared<Sphere>(center1, center2, 50, sphereMaterial));

	// Add dielectric sphere and metal sphere
	world.Add(make_shared<Sphere>(Point3(260, 150, 45), 50, make_shared<DielectricMaterial>(1.5)));
	world.Add(make_shared<Sphere>(Point3(0, 150, 145), 50, make_shared<MetalMaterial>(Color(.8, .8, .9), 1.0)));

	//
	auto boundary = make_shared<Sphere>(Point3(360, 150, 145), 70, make_shared<DielectricMaterial>(1.5));
	world.Add(boundary);
	world.Add(make_shared<ConstantMedium>(boundary, 0.2, Color(.2, .4, .9)));
	boundary = make_shared<Sphere>(Point3(0, 0, 0), 5000, make_shared<DielectricMaterial>(1.5));
	world.Add(make_shared<ConstantMedium>(boundary, .0001, Color(1, 1, 1)));

	// add earth
	auto emat = make_shared<LamberMaterial>(make_shared<ImageTexture>("image/earthmap.jpg"));
	world.Add(make_shared<Sphere>(Point3(400, 200, 400), 100, emat));
	auto pertext = make_shared<NoiseTexture>(.2);
	world.Add(make_shared<Sphere>(Point3(220, 280, 300), 80, make_shared<LamberMaterial>(pertext)));

	// add 1000 small sphere
	Hittable_list boxes2;
	auto white = make_shared<LamberMaterial>(Color(.73, .73, .73));
	int ns = 1000;
	for (int j = 0; j < ns; j++)
	{
		auto pos = (RandomUnitVec() * 165 + Vec3(165, 165, 165)) * .5;
		boxes2.Add(make_shared<Sphere>(pos, 10, white));
	}
	world.Add(make_shared<bvh>(boxes2));
	world.Add(make_shared<Translate>(make_shared<RotateY>(make_shared<bvh>(boxes2), 15), Vec3(-100, 270, 395)));

	Camera cam;
	cam.SetImageHeightAndAspectRatio(height, 1.0);
	cam.SetSampleNum(samplesPerPixel);
	cam.SetMaxDepth(max_depth);
	cam.SetBackground(Color(0, 0, 0));

	cam.SetCameraPara(Vec3(478, 278, -600), Vec3(278, 278, 0), Vec3(0, 1, 0), 40);
	cam.mDefocusAngle = 0.0;

	cam.Render(world);
	cam.WriteBufferToFile(output.c_str());
}

int main(int argc, char** argv) {
	MyTimer t;

	int test = 0;
	if (argc > 1)
		test = std::atoi(argv[1]);
	std::cout << "execute test :" << test << std::endl;
	output = "image" + std::to_string(test) + ".ppm";
	switch (test) {
	case 1:  Process();					break;
	case 2:  CheckerSpheres();          break;
	case 3:  earth();                   break;
	case 4:  PerlinSpheres();           break;
	case 5:  Quads();                   break;
	case 6:  CornelBox();               break;
	case 7:  CornellSmoke();            break;
	case 8:  FinalScene(800, 10000, 40);break;
	default:; FinalScene(400, 50, 4);   break;
	}
}
