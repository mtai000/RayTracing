#pragma once

class Ray {
public:
	Ray() {}
	Ray(const Vec3& origin, const Vec3& direction) : mOrigin(origin), mDirection(direction) {}

	Vec3 GetOrigin() const { return mOrigin; }
	Vec3 GetDirection() const { return mDirection; }

	Vec3 At(double t) const { return mOrigin + mDirection * t; }
private:
	Vec3 mOrigin;
	Vec3 mDirection;
};