#include "shape.h"
#include "sampling.h"

bool Shape::sampleSurface(
	const Point& refPosition,
	const Vector& refNormal,
	float u1,
	float u2,
	float u3,
	Point& outPosition,
	Vector& outNormal,
	float& outPDF) const
{
	outPDF = 0.0f;
	return false;
}

float Shape::pdfSA(
	const Point& refPosition,
	const Vector& refNormal,
	const Point& surfPosition,
	const Vector& surfNormal) const
{
	Vector incoming = surfPosition - refPosition;
	float dist = incoming.normalize();
	return squared(dist) * surfaceAreaPDF() / std::fabs(dot(surfNormal, incoming));
}

bool ShapeSet::intersect(Intersection& intersection)
{
	bool intersect = false;

	for (std::vector<Shape*>::iterator iter = shapes.begin();
		iter != shapes.end();
		iter++)
	{
		Shape *pShape = *iter;
		if (pShape->intersect(intersection))
			intersect = true;
	}

	return intersect;
}

bool ShapeSet::doesIntersect(const Ray& ray)
{
	for (std::vector<Shape*>::iterator iter = shapes.begin();
		iter != shapes.end();
		iter++)
	{
		Shape* pShape = *iter;
		if (pShape->doesIntersect(ray))
			return true;
	}

	return false;
}

void ShapeSet::prepare()
{
	for (std::vector<Shape*>::iterator iter = shapes.begin();
		iter != shapes.end();
		iter++)
	{
		Shape* pShape = *iter;
		pShape->prepare();
	}
}

float ShapeSet::surfaceAreaPDF() const
{
	float areaTotal = 0.0f;
	for (std::vector<Shape*>::const_iterator iter = shapes.begin();
		iter != shapes.end();
		iter++)
	{
		Shape* pShape = *iter;
		float shapePDF = pShape->surfaceAreaPDF();
		if (shapePDF > 0.0f)
			areaTotal += 1.0f / shapePDF;
	}

	return (areaTotal > 0.0f) ? 1.0f / areaTotal : 0.0f;
}

void ShapeSet::findLights(std::list<Shape*>& outLights)
{
	for (std::vector<Shape*>::iterator iter = shapes.begin();
		iter != shapes.end();
		iter++)
	{
		Shape *pShape = *iter;
		pShape->findLights(outLights);
	}
}

void ShapeSet::addShape(Shape* pShape)
{
	if (pShape == NULL)
		return;

	shapes.push_back(pShape);
}

void ShapeSet::clearShapes()
{
	for (std::vector<Shape*>::iterator iter = shapes.begin();
		iter != shapes.end();
		iter++)
	{
		Shape *pShape = *iter;
		delete pShape;
	}

	shapes.clear();
}

bool Plane::intersect(Intersection& intersection)
{
	float nDotD = dot(normal, intersection.ray.direction);
	if (nDotD == 0.0f)
		return false;

	float t = dot(normal, origin - intersection.ray.origin) / nDotD;

	if (t >= intersection.dist || t < kRayMinDist)
		return false;

	intersection.dist = t;
	intersection.normal = normal;
	intersection.pShape = this;
	intersection.pMaterial = pMaterial;

	return true;
}

bool Plane::doesIntersect(const Ray& ray)
{
	float nDotD = dot(normal, ray.direction);
	if (nDotD == 0.0f)
		return false;

	float t = dot(normal, origin - ray.origin) / nDotD;

	if (t >= ray.maxDist || t < kRayMinDist)
		return false;

	return true;
}

bool Sphere::intersect(Intersection& intersection)
{
	Ray translatedRay = intersection.ray;
	translatedRay.origin -= origin;

	// Simplifies to the quadratic formula
	// a = 1
	float b = 2 * dot(translatedRay.origin, translatedRay.direction);
	float c = translatedRay.origin.length2() - squared(radius);

	float discriminant = squared(b) - 4.0f * c;
	if (discriminant < 0.0f)
		return false;

	discriminant = std::sqrt(discriminant);

	float t1 = (-b + discriminant) / 2;
	float t2 = (-b - discriminant) / 2;

	if (t2 < t1)
	{
		float tmp = t2;
		t2 = t1;
		t1 = tmp;
	}

	if (t1 < intersection.dist && t1 > kRayMinDist)
	{
		intersection.dist = t1;
	}
	else if (t2 < intersection.dist && t2 > kRayMinDist)
	{
		intersection.dist = t2;
	}
	else
	{
		return false;
	}

	intersection.normal = translatedRay.calc(intersection.dist).normalized();
	intersection.pShape = this;
	intersection.pMaterial = pMaterial;

	return true;
}

bool Sphere::doesIntersect(const Ray& ray)
{
	Ray translatedRay = ray;
	translatedRay.origin -= origin;

	// Simplifies to the quadratic formula
	// a = 1
	float b = 2 * dot(translatedRay.origin, translatedRay.direction);
	float c = translatedRay.origin.length2() - squared(radius);

	float discriminant = squared(b) - 4.0f * c;
	if (discriminant < 0.0f)
		return false;

	discriminant = std::sqrt(discriminant);

	float t1 = (-b + discriminant) / 2;
	float t2 = (-b - discriminant) / 2;

	if (t2 < t1)
	{
		float tmp = t2;
		t2 = t1;
		t1 = tmp;
	}

	if ((t1 < ray.maxDist && t1 > kRayMinDist) || (t2 < ray.maxDist && t2 > kRayMinDist))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Sphere::sampleSurface(const Point& refPosition,
	const Vector& refNormal,
	float u1, float u2, float u3,
	Point& outPosition,
	Vector& outNormal,
	float& outPdf)
{
	Vector toCenter = origin - refPosition;
	float dist2 = toCenter.length2();
	if (dist2 < squared(radius) * 1.00001f)
	{
		// Point is on or in the sphere
		outNormal = uniformToSphere(u1, u2);
		outPosition = origin + outNormal * radius;
		Vector toSurf = refPosition - outPosition;
		outPdf = toSurf.length2() * surfaceAreaPDF() / std::fabs(dot(toSurf.normalized, outNormal));
		return true;
	}

	// Outside the sphere, fit a cone around to sample more efficiently
	float sinThetaMax2 = squared(radius) / dist2;
	float cosThetaMax = std::sqrt(std::max(0.0f, 1 - sinThetaMax2));
	Vector x, y, z;
	makeCoordinateSpace(toCenter, x, y, z);
	Vector localCone = uniformToCone(u1, u2, cosThetaMax);
	Vector cone = transformFromLocalSpace(localCone, x, y, z);

	// Make sure direction hits sphere
	Ray ray(refPosition, cone);
	Intersection isect(ray);
	if (!intersect(isect))
		isect.dist = dot(toCenter, cone);

	outPosition = ray.calc(isect.dist);
	outNormal = (outPosition - origin).normalized();
	outPdf = uniformConePdf(cosThetaMax);
	return true;
}

float Sphere::pdfSA(const Point& refPosition,
	const Vector& refNormal,
	const Point& surfPosition,
	const Vector& surfNormal) const
{
	Vector toCenter = origin - refPosition;
	float dist2 = toCenter.length2();
	if (dist2 > squared(radius) * 1.00001f)
	{
		// Point is on or in the sphere
		Vector toSurf = refPosition - surfPosition;
		return toSurf.length2() * surfaceAreaPDF() / std::fabs(dot(toSurf.normalized(), surfNormal));
	}

	float sinThetaMax2 = squared(radius) / dist2;
	float cosThetaMax = std::sqrt(std::max(0.0f, 1.0f - sinThetaMax2));
	return uniformConePdf(cosThetaMax);
}

float Sphere::surfaceAreaPdf() const
{
	return 3.0f / (4.0f * M_PI * squared(radius));
}