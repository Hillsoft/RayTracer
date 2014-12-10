#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "shape.h"

class Light : public Shape
{
public:
	Light(const Color& color, float power) : color(color), power(power), material(color, power) { }

	virtual ~Light() { }

	virtual void findLights(std::list<Shape*>&outLightList);
	virtual bool isLight() const;

	virtual Color emitted() const;

	virtual float intersectPdf(const Intersection& isect) = 0;

protected:
	Color color;
	float power;
	Emitter material;
};

class RectangleLight : public Light
{
public:
	RectangleLight(const Point& pos,
		const Vector& side1,
		const Vector& side2,
		const Color& color,
		float power)
		: Light(color, power), origin(pos), side1(side1), side2(side2) { }

	virtual ~RectangleLight() { }

	virtual bool intersect(Intersection& intersection);
	virtual bool doesIntersect(const Ray& ray);

	virtual bool sampleSurface(const Point& surfPosition,
		const Vector& surfNormal,
		float u1, float u2, float u3,
		Point& outPosition,
		Vector& outNormal,
		float& outPdf);

	virtual float intersectPdf(const Intersection& isect);

protected:
	Point origin;
	Vector side1, side2;
};

class ShapeLight : public Light
{
public:
	ShapeLight(Shape *pShape,
		const Color& color,
		float power)
		: Light(color, power), pShape(pShape) { }

	virtual ~ShapeLight()
	{
		delete pShape;
	}

	virtual bool intersect(Intersection& intersection);
	virtual bool doesIntersect(const Ray& ray);

	virtual bool sampleSurface(const Point& surfPosition,
		const Point& surfNormal,
		float u1, float u2, float u3,
		Point& outPosition,
		Vector& outNormal,
		float& outPdf);

	virtual float intersectPdf(const Intersection& isect);

protected:
	Shape* pShape;
};

#endif