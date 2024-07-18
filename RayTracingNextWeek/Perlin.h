#pragma once
#include "rtweekend.h"

class Perlin {
public:
	Perlin() {
		randVec = new Vec3[point_count];
		for (int i = 0; i < point_count; i++) {
			randVec[i] = RandomUnitVec();
		}

		perm_x = perlin_generate_perm();
		perm_y = perlin_generate_perm();
		perm_z = perlin_generate_perm();
	}

	~Perlin() {
		delete[] randVec;
		delete[] perm_x;
		delete[] perm_y;
		delete[] perm_z;
		delete[] randfloat;
	}


	double noise(const Point3& p) const {
		auto i = int(4 * p.x()) & 255;
		auto j = int(4 * p.y()) & 255;
		auto k = int(4 * p.z()) & 255;

		return randfloat[perm_x[i] ^ perm_y[j] ^ perm_z[k]];
	}

	double SmoothNoise(const Point3& p) const {
		auto i = int(std::floor(p.x()));
		auto j = int(std::floor(p.y()));
		auto k = int(std::floor(p.z()));

		auto u = p.x() - i;
		auto v = p.y() - j;
		auto w = p.z() - k;

		Vec3 c[2][2][2];

		for (int di = 0; di < 2; di++)
			for (int dj = 0; dj < 2; dj++)
				for (int dk = 0; dk < 2; dk++)
				{
					c[di][dj][dk] = randVec[
						perm_x[(i + di) & 255] ^
							perm_y[(j + dj) & 255] ^
							perm_z[(k + dk) & 255]
					];
				}
		return PerlinInterp(c, u, v, w);
	}

	double Turb(const Point3& p, int depth) const {
		auto accum = 0.0;
		auto temp_p = p;
		auto weight = 1.0;

		for (int i = 0; i < depth; i++) {
			accum += weight * SmoothNoise(temp_p);
			weight *= 0.5;
			temp_p = temp_p * 2;
		}

		return fabs(accum);
	}
private:
	static const int point_count = 256;
	double* randfloat;
	Vec3* randVec;
	int* perm_x;
	int* perm_y;
	int* perm_z;

	static int* perlin_generate_perm() {
		auto p = new int[point_count];
		for (int i = 0; i < point_count; i++)
			p[i] = i;

		permute(p, point_count);
		return p;
	}

	static void permute(int* p, int n) {
		for (int i = n - 1; i > 0; i--) {
			int target = int(random_double(0, i - 0.000001));
			std::swap(p[i], p[target]);
		}
	}

	static double PerlinInterp(const Vec3 c[2][2][2], double u, double v, double w)
	{
		auto uu = u * u * (3 - 2 * u);
		auto vv = v * v * (3 - 2 * v);
		auto ww = w * w * (3 - 2 * w);

		auto accum = 0.0;

		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
				{
					Vec3 weight_v(u - i, v - j, w - k);
					accum += (i * uu + (1 - i) * (1 - uu))
						* (j * vv + (1 - j) * (1 - vv))
						* (k * ww + (1 - k) * (1 - ww))
						* dot(c[i][j][k], weight_v);
				}

		return accum;
	}
	static double TrilinearInterp(double c[2][2][2], double u, double v, double w) {
		auto accum = 0.0;
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
				{
					accum += (i * u + (1 - i) * (1 - u))
						* (j * v + (1 - j) * (1 - v))
						* (k * w + (1 - k) * (1 - w))
						* c[i][j][k];
				}

		return accum;
	}
};