#ifndef __SHAPE_H__
#define __SHAPE_H__

#include <list>
#include <vector>

#include "maths.h"
#include "ray.h"
#include "material.h"

class Shape
{
public:
	virtual ~Shape() {}

	virtual bool intersect(Intersection& intersection) = 0;
	virtual bool doesIntersect(const Ray& ray) = 0;

	virtual void prepare() { }

	// Usually used for lights when sampling
	virtual bool sampleSurface(
		const Point& refPosition,
		const Vector& refNormal,
		float u1,
		float u2,
		float u3,
		Point& outPosition,
		Vector& outNormal,
		float& outPDF) const;

	virtual float pdfSA(
		const Point& refPosition,
		const Vector& refNormal,
		const Point& surfPosition,
		const Vector& surfNormal) const;

	virtual float surfaceAreaPDF() const
	{
		return 0.0f;
	}

	// Finds all lights within the scene
	virtual void findLights(std::list<Shape*>& outLights) { }

	virtual bool isLight() const { return false; }
};

class ShapeSet : public Shape
{
public:
	ShapeSet() : shapes() {}

	virtual ~ShapeSet() { clearShapes(); }

	virtual bool intersect(Intersection& intersection);
	virtual bool doesIntersect(const Ray& ray);

	virtual void prepare();

	virtual float surfaceAreaPDF() const;

	virtual void findLights(std::list<Shape*>& outLights);

	void addShape(Shape* pShape);
	void clearShapes();

protected:
	std::vector<Shape*> shapes;
};

class Plane : public Shape
{
public:
	Plane() : origin(0.0f), normal(0.0f, 0.0f, 1.0f), pMaterial(NULL) {}
	Plane(const Point& origin, const Vector& normal, Material* pMaterial)
		: origin(origin), normal(normal.normalized()), pMaterial(pMaterial) {}

	virtual ~Plane() { }

	virtual bool intersect(Intersection& intersection);
	virtual bool doesIntersect(const Ray& ray);

protected:
	Point origin;
	Vector normal;
	Material* pMaterial;
};

class Sphere : public Shape
{
public:
	Sphere() : origin(0.0f), radius(1.0f), pMaterial(NULL) {}
	Sphere(const Point& origin, float radius, Material* pMaterial)
		: origin(origin), radius(radius), pMaterial(pMaterial) {}

	virtual ~Sphere() { }

	virtual bool intersect(Intersection& intersection);
	virtual bool doesIntersect(const Ray& ray);

protected:
	Point origin;
	float radius;
	Material* pMaterial;
};

#endif