#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "ray.h"

class Camera
{
public:
	Camera() { }

	virtual ~Camera() { }

	virtual Ray makeRay(float xScreen, float yScreen, float lensU, float lensV) const = 0;
};

class PerspectiveCamera : public Camera
{
public:
	// lensRadius = 0 to disable DOF
	PerspectiveCamera(float fieldOfViewInDegrees,
		const Point& origin,
		const Vector& target,
		const Vector& targetUpDirection,
		float focalDistance,
		float lensRadius);

	virtual ~PerspectiveCamera() { }

	virtual Ray makeRay(float xScreen, float yScreen, float lensU, float lensV) const;

protected:
	Point origin;
	Vector forward;
	Vector up;
	Vector right;
	float tanFov;
	float focalDistance;
	float lensRadius;
};

#endif