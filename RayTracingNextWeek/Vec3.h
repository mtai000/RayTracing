#pragma once

inline double random_double(double min, double max);
constexpr  double Epsilon = 1e-8;
class Vec3 {
public:
	double e[3];

	Vec3() : e{ 0.0,0.0,0.0 } {}
	Vec3(double x, double y, double z) : e{ x,y,z } {}

	double x() const { return e[0]; }
	double y() const { return e[1]; }
	double z() const { return e[2]; }

	double operator[] (int i) const { return e[i]; }
	double& operator[] (int i) { return e[i]; }

	Vec3 operator- ()  const { return Vec3(-e[0], -e[1], -e[2]); }

	Vec3& operator+= (const Vec3& v) {
		e[0] += v[0];
		e[1] += v[1];
		e[2] += v[2];
		return *this;
	}

	Vec3& operator*= (const Vec3& v) {
		e[0] *= v[0];
		e[1] *= v[1];
		e[2] *= v[2];
		return *this;
	}

	Vec3& operator/=(double val) {
		double div = 1 / val;
		e[0] *= div;
		e[1] *= div;
		e[2] *= div;
		return *this;
	}

	double length() const {
		return sqrt(length_2());
	}

	double length_2() const {
		return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
	}

	Vec3& normalized()
	{
		auto div = 1 / length();
		e[0] *= div;
		e[1] *= div;
		e[2] *= div;
		return *this;
	}

	bool Near_Zero()
	{
		return fabs(e[0]) < Epsilon && fabs(e[1]) < Epsilon && fabs(e[2]) < Epsilon;
	}
};


inline Vec3 operator+ (const Vec3& u, const Vec3& v) {
	return Vec3(u[0] + v[0], u[1] + v[1], u[2] + v[2]);
}

inline Vec3 operator- (const Vec3& u, const Vec3& v) {
	return Vec3(u[0] - v[0], u[1] - v[1], u[2] - v[2]);
}

inline Vec3 operator* (const Vec3& u, double _d) {
	return Vec3(u[0] * _d, u[1] * _d, u[2] * _d);
}

inline Vec3 operator* (double _d, const Vec3& u) {
	return Vec3(u[0] * _d, u[1] * _d, u[2] * _d);
}

inline Vec3 operator/ (const Vec3& u, double _d) {
	auto div = 1 / _d;
	return Vec3(u[0] * div, u[1] * div, u[2] * div);
}

inline Vec3 operator/ (double _d, const Vec3& u) {
	return Vec3(_d / u[0], _d / u[1],  _d / u[2]);
}

inline std::ostream& operator<< (std::ostream& outStream, const Vec3& v) {
	outStream << v[0] << ' ' << v[1] << ' ' << v[2] << std::endl;
	return outStream;
}

inline double dot(const Vec3& u, const Vec3& v) {
	return u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
}

inline Vec3 cross(const Vec3& u, const Vec3 v) {
	return Vec3(u[1] * v[2] - u[2] * v[1]
			  , u[2] * v[0] - v[2] * u[0]
			  , u[0] * v[1] - v[0] * u[1]);
}

inline Vec3 normailze(const Vec3& v) {
	auto ret = v;
	auto div = 1 / v.length();
	return Vec3(ret[0] *= div, ret[1] *= div, ret[2] *= div);
}

inline Vec3 RandomVec(double min, double max) {
	return Vec3(random_double(min, max), random_double(min, max), random_double(min, max));
}

inline Vec3 RandomUnitVec() {
	while (true)
	{
		Vec3 v = RandomVec(-1.0, 1.0);
		if (v.length_2() < 1)
			return v.normalized();
	}
}

inline Vec3 HemisphereVec(const Vec3& normal) {
	auto direction = RandomUnitVec();
	if (dot(normal, direction) < 0) {
		return -direction;
	}
	else
	{
		return direction;
	}
}

inline bool Near_Zero(const Vec3& v)
{
	return fabs(v[0]) < Epsilon && fabs(v[1]) < Epsilon && fabs(v[2]) < Epsilon;
}

inline Vec3 Reflect(const Vec3& in, const Vec3 normal)
{
	return in - dot(in, normal) * 2 * normal;
}

// in and normal is normalized vector
inline Vec3 Refract(const Vec3& in, const Vec3& normal, double refract_ratio)
{
	// in,normal
	// cos theta * n + sin thata * b = d
	// n : (0,1)  b ( -x, 0) , in ( -x1 ,-y1) , y1 = cos theta
	// d = b * sin theta  - n * cos theta 
	// b = (d + n * cos theta) / sin theta;
	
	//cal ret perpendicular to normal  : b
	auto cos_theta = fmin(dot(-in, normal), 1.0);
	Vec3 ret_prep = refract_ratio * (in + cos_theta * normal);

	//cal ret parallel to normal
	Vec3 ret_parallel = -sqrt(1 - ret_prep.length_2()) * normal;
	return ret_prep + ret_parallel;
}

typedef Vec3 Point3;