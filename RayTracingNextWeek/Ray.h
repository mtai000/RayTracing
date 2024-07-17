#pragma once

class Ray {
public:
	Ray() :mOrigin(Vec3(0, 0, 0)), mDirection(Vec3(0, 0, -1)), mTime(0) {}
	Ray(const Vec3& origin, const Vec3& direction, double time) : mOrigin(origin), mDirection(direction), mTime(time) {}

	Vec3 GetOrigin() const { return mOrigin; }
	Vec3 GetDirection() const { return mDirection; }

	Vec3 At(double t) const { return mOrigin + mDirection * t; }
	double GetTime() const { return mTime; }
private:
	Vec3 mOrigin;
	Vec3 mDirection;
	double mTime;
};