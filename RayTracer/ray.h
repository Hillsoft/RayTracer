#ifndef __RAY_H__
#define __RAY_H__

#include "maths.h"

// Prevents self intersection
const float kRayMinDist = 0.0001f;

const float kRayMaxDist = 1.0e30f;

struct Ray
{
	Point origin;
	Vector direction;
	float maxDist;

	Ray() : origin(), direction(0.0f, 0.0f, 1.0f), maxDist(kRayMaxDist) {}
	Ray(const Ray& r) : origin(r.origin), direction(r.direction), maxDist(r.maxDist) {}
	Ray(const Point& origin, const Vector& direction, float maxDist = kRayMaxDist)
		: origin(origin), direction(direction.normalized()), maxDist(maxDist) {}

	inline Ray& operator =(const Ray& r)
	{
		origin = r.origin;
		direction = r.direction;
		maxDist = r.maxDist;
		return *this;
	}

	Point calc(float dist) const;
};

class Shape;
class Material;

struct Intersection
{
	Ray ray;
	float dist;
	Shape* pShape;
	Material* pMaterial;
	Vector normal;

	Intersection() : ray(), dist(kRayMaxDist), pShape(NULL), pMaterial(NULL), normal() {}
	Intersection(const Intersection& i)
		: ray(i.ray),
		dist(i.dist),
		pShape(i.pShape),
		pMaterial(i.pMaterial),
		normal(i.normal) {}
	Intersection(const Ray& r)
		: ray(r),
		dist(r.maxDist),
		pShape(NULL),
		pMaterial(NULL),
		normal() {}

	inline Intersection& operator =(const Intersection& i)
	{
		ray = i.ray;
		dist = i.dist;
		pShape = i.pShape;
		pMaterial = i.pMaterial;
		normal = i.normal;
	}

	bool intersected() const;
	Point position() const;
};

#endif