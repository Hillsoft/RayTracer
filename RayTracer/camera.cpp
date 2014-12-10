#include "camera.h"
#include "sampling.h"

PerspectiveCamera::PerspectiveCamera(float fieldOfViewInDegrees,
	const Point& origin,
	const Vector& target,
	const Vector& targetUpDirection,
	float focalDistance,
	float lensRadius)
	: origin(origin),
	forward((target - origin).normalized()),
	tanFov(std::tan(fieldOfViewInDegrees * M_PI / 180.0f)),
	focalDistance(focalDistance),
	lensRadius(lensRadius)
{
	right = cross(forward, targetUpDirection).normalized();
	up = cross(right, forward);
}

Ray PerspectiveCamera::makeRay(float xScreen, float yScreen, float lensU, float lensV) const
{
	Ray ray;
	ray.origin = origin;
	ray.direction = forward + right * ((xScreen - 0.5f) * tanFov) + up * ((yScreen - 0.5f) * tanFov);
	ray.direction.normalize();

	if (lensRadius > 0.0f)
	{
		// Modify for DOF
		float horizontalShift = 0.0f, verticalShift = 0.0f;
		float nearDistance = 0.0f;

		uniformToUniformDisc(lensU, lensV, horizontalShift, verticalShift);
		horizontalShift *= lensRadius;
		verticalShift *= lensRadius;

		Vector localRayDirection((xScreen - 0.5f) * tanFov,
			(yScreen - 0.5f) * tanFov,
			1.0f);
		localRayDirection.normalize();

		float focusT = (focalDistance - nearDistance) / localRayDirection.z;
		Point focusPoint = origin + ray.direction * focusT;

		ray.origin += right * horizontalShift + up * verticalShift;

		ray.direction = focusPoint - ray.origin;
		ray.direction.normalize();
	}

	return ray;
}