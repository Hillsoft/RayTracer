#ifndef __SAMPLING_H__
#define __SAMPLING_H__

#include "maths.h"

inline void concentricSampleDisc(float u1, float u2, float& outDx, float& outDy)
{
	float r, theta;

	float sx = 2.0f * u1 - 1.0f;
	float sy = 2.0f * u2 - 1.0f;

	if (sx == 0.0f && sy == 0.0f)
	{
		outDx = 0.0f;
		outDy = 0.0f;
		return;
	}
	if (sx >= -sy)
	{
		if (sx > sy)
		{
			r = sx;
			if (sy > 0.0f)
				theta = sy / r;
			else
				theta = 8.0f + sy / r;
		}
		else
		{
			r = sy;
			theta = 2.0f - sx / r;
		}
	}
	else
	{
		if (sx <= sy)
		{
			r = -sx;
			theta = 4.0f - sy / r;
		}
		else
		{
			r = -sy;
			theta = 6.0f + sx / r;
		}
	}

	theta *= M_PI / 4.0f;
	outDx = r * std::cos(theta);
	outDy = r * std::sin(theta);
}

inline Vector uniformToSphere(float u1, float u2)
{
	float z = 1.0f - 2.0f * u1;

	float radius = std::sqrt(std::max(0.0f, 1.0f - squared(z)));

	float phi = M_PI * 2.0f * u2;

	return Vector(radius * std::cos(phi), radius * std::sin(phi), z);
}

inline void uniformToUniformDisc(float u1, float u2, float& uDx, float uDy)
{
	float radius = std::sqrt(u1);
	float theta = M_PI * 2.0f * u2;

	uDx = radius * std::cos(theta);
	uDy = radius * std::sin(theta);
}

inline Vector uniformToHemisphere(float u1, float u2)
{
	float radius = std::sqrt(std::max(0.0f, 1.0f - squared(u1)));
	float phi = M_PI * 2.0f * u2;
	return Vector(radius * std::cos(phi),
		radius * std::sin(phi),
		u1);
}

inline Vector uniformToCosineHemisphere(float u1, float u2)
{
	float discX, discY;

	concentricSampleDisc(u1, u2, discX, discY);
	float z = std::sqrt(std::max(0.0f, 1.0f - squared(discX) - squared(discY)));
	return Vector(discX, discY, z);
}

inline Vector uniformToCone(float u1, float u2, float cosThetaMax)
{
	float cosTheta = u1 * (cosThetaMax - 1.0f) + 1.0f;
	float sinTheta = std::sqrt(std::max(0.0f, 1.0f - squared(cosTheta)));
	float phi = u2 * M_PI * 2.0f;
	return Vector(std::cos(phi) * sinTheta, std::sin(phi) * sinTheta, cosTheta);
}

inline float uniformConePdf(float cosThetaMax)
{
	return cosThetaMax >= 1.0f ? 0.0f : 1.0f / (2.0f * M_PI * (1.0f - cosThetaMax));
}

#endif