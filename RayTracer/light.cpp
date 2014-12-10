#include "light.h"

void Light::findLights(std::list<Shape*>& outLightList)
{
	outLightList.push_back(this);
}

bool Light::isLight() const
{
	return true;
}

Color Light::emitted() const
{
	return color * power;
}

bool RectangleLight::intersect(Intersection& intersection)
{
	Vector normal = cross(side1, side2).normalized();
	float nDotD = dot(normal, intersection.ray.direction);
	if (nDotD == 0.0f)
		return false;

	float t = dot(origin - intersection.ray.origin, normal) / nDotD;

	if (t >= intersection.dist || t < kRayMinDist)
		return false;

	Vector side1Norm = side1;
	Vector side2Norm = side2;
	float side1Length = side1Norm.normalize();
	float side2Length = side2Norm.normalize();

	Point worldPoint = intersection.ray.calc(t);
	Point relativePoint = worldPoint - origin;
	Point localPoint = Point(dot(relativePoint, side1Norm),
		dot(relativePoint, side2Norm),
		0.0f);

	if (localPoint.x < 0.0f || localPoint.x > side1Length ||
		localPoint.y < 0.0f || localPoint.y > side2Length)
		return false;

	intersection.dist = t;
	intersection.pShape = this;
	intersection.pMaterial = &material;
	intersection.normal = normal;

	if (dot(intersection.normal, intersection.ray.direction) > 0.0f)
		intersection.normal *= -1.0f;

	return true;
}

bool RectangleLight::doesIntersect(const Ray& ray)
{
	Vector normal = cross(side1, side2).normalized();
	float nDotD = dot(normal, ray.direction);
	if (nDotD == 0.0f)
		return false;

	float t = dot(origin - ray.origin, normal) / nDotD;

	if (t >= ray.maxDist || t < kRayMinDist)
		return false;

	Vector side1Norm = side1;
	Vector side2Norm = side2;
	float side1Length = side1Norm.normalize();
	float side2Length = side2Norm.normalize();

	Point worldPoint = ray.calc(t);
	Point relativePoint = worldPoint - origin;
	Point localPoint = Point(dot(relativePoint, side1Norm),
		dot(relativePoint, side2Norm),
		0.0f);

	if (localPoint.x < 0.0f || localPoint.x > side1Length ||
		localPoint.y < 0.0f || localPoint.y > side2Length)
		return false;

	return true;
}

bool RectangleLight::sampleSurface(const Point& surfPosition,
	const Vector& surfNormal,
	float u1, float u2, float u3,
	Point& outPosition,
	Vector& outNormal,
	float& outPdf)
{
	outPosition = origin + side1 * u1 + side2 * u2;
	Vector outgoing = surfPosition - outPosition;
	float dist = outgoing.normalize();
	outNormal = cross(side1, side2);
	float area = outNormal.normalize();

	if (dot(outNormal, outgoing) < 0.0f)
		outNormal *= -1.0f;
	outPdf = squared(dist) / (area * std::fabs(dot(outNormal, outgoing)));

	// Really big PDFs will cause issues later, remove them now
	if (outPdf > 1.0e10f)
	{
		outPdf = 0.0f;
		return false;
	}

	return true;
}

float RectangleLight::intersectPdf(const Intersection& isect)
{
	if (isect.pShape == this)
	{
		float pdf = squared(isect.dist) / (std::fabs(dot(isect.normal, -isect.ray.direction)) * cross(side1, side2).length());

		// Really big PDFs will cause issues later, remove them now
		if (pdf > 1.0e10f)
			return 0.0f;
		return pdf;
	}

	return 0.0f;
}

bool ShapeLight::intersect(Intersection& intersection)
{
	if (pShape->intersect(intersection))
	{
		intersection.pMaterial = &material;
		intersection.pShape = this;
		return true;
	}

	return false;
}

bool ShapeLight::doesIntersect(const Ray& ray)
{
	return pShape->doesIntersect(ray);
}

bool ShapeLight::sampleSurface(const Point& surfPosition,
	const Point& surfNormal,
	float u1, float u2, float u3,
	Point& outPosition,
	Vector& outNormal,
	float& outPdf)
{
	if (!pShape->sampleSurface(surfPosition, surfNormal, u1, u2, u3,
		outPosition, outNormal, outPdf))
	{
		outPdf = 0.0f;
		return false;
	}

	// Discard points in back of light
	if (dot(outNormal, surfPosition - outPosition) < 0.0f)
		return false;

	return true;
}

float ShapeLight::intersectPdf(const Intersection& isect)
{
	if (isect.pShape == this)
	{
		// TODO: not really correct, but its ok as this function isn't used yet ;)
		return pShape->pdfSA(isect.ray.origin, isect.ray.direction,
			isect.position(), isect.normal);
	}
	return 0.0f;
}