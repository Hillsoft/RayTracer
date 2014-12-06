#include "ray.h"

Point Ray::calc(float dist) const
{
	return origin + direction * dist;
}

bool Intersection::intersected() const
{
	return !(pShape == NULL);
}

Point Intersection::position() const
{
	return ray.calc(dist);
}