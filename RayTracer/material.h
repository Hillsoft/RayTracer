#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "maths.h"

class Brdf
{
public:
	Brdf() { }

	virtual ~Brdf() { }

	virtual float evaluateSA(const Vector& incoming, const Vector& outgoing, const Vector& normal, float& outPdf) const = 0;
	virtual float evaluatePSA(const Vector& incoming, const Vector& outgoing, const Vector& normal, float& outPdf) const
	{
		float reflectance = evaluateSA(incoming, outgoing, normal, outPdf);

		// Convert to projected solid angle
		outPdf /= std::fabs(dot(incoming, normal));
		return reflectance;
	}

	virtual float sampleSA(Vector& outIncoming, const Vector& outgoing, const Vector& normal, float u1, float u2, float& outPdf) const = 0;
	virtual float samplePSA(Vector& outIncoming, const Vector& outgoing, const Vector& normal, float u1, float u2, float& outPdf) const
	{
		float reflectance = sampleSA(outIncoming, outgoing, normal, u1, u2, outPdf);

		// Convert to projected solid angle
		outPdf /= std::fabs(dot(outIncoming, normal));
		return reflectance;
	}

	virtual float pdfSA(const Vector& incoming, const Vector& outgoing, const Vector& normal) const = 0;
	virtual float pdfPSA(const Vector& incoming, const Vector& outgoing, const Vector& normal) const
	{
		return pdfSA(incoming, outgoing, normal) / std::fabs(dot(incoming, normal));
	}

	virtual bool isDiracDistribution() const { return false; }
};

class Lambert : public Brdf
{
public:
	Lambert() : Brdf() { }

	virtual ~Lambert() { }

	virtual float evaluateSA(const Vector& incoming, const Vector& outgoing, const Vector& normal, float& outPdf) const;
	virtual float evaluatePSA(const Vector& incoming, const Vector& outgoing, const Vector& normal, float& outPdf) const;

	virtual float sampleSA(Vector& outIncoming, const Vector& outgoing, const Vector& normal, float u1, float u2, float& outPdf) const;
	virtual float samplePSA(Vector& outIncoming, const Vector& outgoing, const Vector& normal, float u1, float u2, float& outPdf) const;

	virtual float pdfSA(const Vector& incoming, const Vector& outgoing, const Vector& normal) const;
	virtual float pdfPSA(const Vector& incoming, const Vector& outgoing, const Vector& normal) const;
};

class Glossy : public Brdf
{
public:
	Glossy(float roughness) : Brdf(), exponent(1.0f / squared(roughness)) { }

	virtual ~Glossy() { }

	float schlickFresnel(float reflectionIncidentToNormal, float cosTheta) const;

	virtual float evaluateSA(const Vector& incoming, const Vector& outgoing, const Vector& normal, float& outPdf) const;
	virtual float evaluatePSA(const Vector& incoming, const Vector& outgoing, const Vector& normal, float& outPdf) const;

	virtual float sampleSA(Vector& outIncoming, const Vector& outgoing, const Vector& normal, float u1, float u2, float& outPdf) const;
	virtual float samplePSA(Vector& outIncoming, const Vector& outgoing, const Vector& normal, float u1, float u2, float& outPdf) const;

	virtual float pdfSA(const Vector& incoming, const Vector& outgoing, const Vector& normal) const;
	virtual float pdfPSA(const Vector& incoming, const Vector& outgoing, const Vector& normal) const;

protected:
	float exponent;
};


class Material
{
public:
	virtual ~Material() { }

	virtual Color emittance() { return Color(); }
	virtual Color evaluate(
		const Point& position,
		const Vector& normal,
		const Vector& outgoingRayDirection,
		Brdf*& pBrdfChosen,
		float& brdfWeight) = 0;
};

class DiffuseMaterial : public Material
{
public:
	DiffuseMaterial(const Color& color) : color(color), lambert() { }

	virtual ~DiffuseMaterial() { }

	virtual Color evaluate(
		const Point& position,
		const Vector& normal,
		const Vector& outgoingRayDirection,
		Brdf*& pBrdfChosen,
		float& brdfWeight);

protected:
	Color color;
	Lambert lambert;
};

class GlossyMaterial : public Material
{
public:
	GlossyMaterial(const Color& color, float roughness) : color(color), glossy(roughness) { }

	virtual ~GlossyMaterial() { }

	virtual Color evaluate(
		const Point& position,
		const Vector& normal,
		const Vector& outgoingRayDirection,
		Brdf*& pBrdfChosen,
		float& brdfWeight);

protected:
	Color color;
	Glossy glossy;
};

class Emitter : public Material
{
public:
	Emitter(const Color& color, float power) : color(color), power(power) { }

	virtual ~Emitter() { }
	
	virtual Color emittance();

	virtual Color evaluate(
		const Point& position,
		const Vector& normal,
		const Vector& outgoingRayDirection,
		Brdf*& pBrdfChosen,
		float& brdfWeight);

protected:
	Color color;
	float power;
};

#endif