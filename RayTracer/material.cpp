#include "material.h"

#include "sampling.h"

float Lambert::evaluateSA(const Vector& incoming, const Vector& outgoing, const Vector& normal, float& outPdf) const
{
	float nDotI = dot(incoming, normal);
	float nDotO = dot(outgoing, normal);
	if ((nDotI > 0.0f && nDotO > 0.0f) ||
		(nDotI < 0.0f && nDotO < 0.0f))
	{
		outPdf = 0.0f;
		return 0.0f;
	}

	outPdf = std::fabs(nDotI) / M_PI;
	return 1.0f / M_PI;
}

float Lambert::evaluatePSA(const Vector& incoming, const Vector& outgoing, const Vector& normal, float& outPdf) const
{
	float nDotI = dot(incoming, normal);
	float nDotO = dot(outgoing, normal);
	if ((nDotI > 0.0f && nDotO > 0.0f) ||
		(nDotI < 0.0f && nDotO < 0.0f))
	{
		outPdf = 0.0f;
		return 0.0f;
	}

	outPdf = 1.0f / M_PI;
	return 1.0f / M_PI;
}

float Lambert::sampleSA(Vector& outIncoming, const Vector& outgoing, const Vector& normal, float u1, float u2, float& outPdf) const
{
	Vector localIncoming = -uniformToCosineHemisphere(u1, u2);

	Vector x, y, z;
	makeCoordinateSpace(normal, x, y, z);
	outIncoming = transformFromLocalSpace(localIncoming, x, y, z);

	if (dot(outgoing, normal) < 0.0f)
		outIncoming *= -1.0f;

	outPdf = std::fabs(dot(-outIncoming, normal)) / M_PI;
	return 1.0f / M_PI;
}

float Lambert::samplePSA(Vector& outIncoming, const Vector& outgoing, const Vector& normal, float u1, float u2, float& outPdf) const
{
	Vector localIncoming = -uniformToCosineHemisphere(u1, u2);

	Vector x, y, z;
	makeCoordinateSpace(normal, x, y, z);
	outIncoming = transformFromLocalSpace(localIncoming, x, y, z);

	if (dot(outgoing, normal) < 0.0f)
		outIncoming *= -1.0f;

	outPdf = 1.0f / M_PI;
	return 1.0f / M_PI;
}

float Lambert::pdfSA(const Vector& incoming, const Vector& outgoing, const Vector& normal) const
{
	float nDotI = dot(incoming, normal);
	float nDotO = dot(outgoing, normal);
	if ((nDotI > 0.0f && nDotO > 0.0f) ||
		(nDotI < 0.0f && nDotO < 0.0f))
	{
		return 0.0f;
	}

	return std::fabs(nDotI) / M_PI;
}

float Lambert::pdfSA(const Vector& incoming, const Vector& outgoing, const Vector& normal) const
{
	float nDotI = dot(incoming, normal);
	float nDotO = dot(outgoing, normal);
	if ((nDotI > 0.0f && nDotO > 0.0f) ||
		(nDotI < 0.0f && nDotO < 0.0f))
	{
		return 0.0f;
	}

	return 1.0f / M_PI;
}

float Glossy::schlickFresnel(float reflectionIncidentToNormal, float cosTheta) const
{
	return reflectionIncidentToNormal +
		(1.0f - reflectionIncidentToNormal) *
		std::pow(std::max(1.0f - cosTheta, 0.0f), 5);
}

float Glossy::evaluateSA(const Vector& incoming, const Vector& outgoing, const Vector& normal, float& outPdf) const
{
	float nDotI = dot(incoming, normal);
	float nDotO = dot(incoming, normal);
	if ((nDotI > 0.0f && nDotO > 0.0f) ||
		(nDotI < 0.0f && nDotO < 0.0f))
	{
		outPdf = 0.0f;
		return 0.0f;
	}

	Vector half;
	if (dot(outgoing, incoming) > 0.999f)
		half = normal;
	else
		half = (outgoing - incoming).normalized();

	float fresnel = 1.0f;

	float d = (exponent + 1.0f) * std::pow(std::fabs(dot(normal, half)), exponent) / (2.0f * M_PI);
	float result = fresnel * d / (4.0f * std::fabs(nDotO - nDotI - nDotO * -nDotI));
	outPdf = d / (4.0f * std::fabs(dot(outgoing, half)));
	return result;
}

float Glossy::evaluatePSA(const Vector& incoming, const Vector& outgoing, const Vector& normal, float& outPdf) const
{
	float nDotI = dot(incoming, normal);
	float nDotO = dot(incoming, normal);
	if ((nDotI > 0.0f && nDotO > 0.0f) ||
		(nDotI < 0.0f && nDotO < 0.0f))
	{
		outPdf = 0.0f;
		return 0.0f;
	}

	Vector half;
	if (dot(outgoing, incoming) > 0.999f)
		half = normal;
	else
		half = (outgoing - incoming).normalized();

	float fresnel = 1.0f;

	float d = (exponent + 1.0f) * std::pow(std::fabs(dot(normal, half)), exponent) / (2.0f * M_PI);
	float result = fresnel * d / (4.0f * std::fabs(nDotO - nDotI - nDotO * -nDotI));
	outPdf = d / (4.0f * std::fabs(dot(outgoing, half))) * std::fabs(nDotI);
	return result;
}

float Glossy::sampleSA(Vector& outIncoming, const Vector& outgoing, const Vector& normal, float u1, float u2, float& outPdf) const
{
	float phi = 2.0f * M_PI * u1;
	float cosTheta = std::pow(1.0f - u2, 1.0f / (exponent + 1.0f));
	float sin2Theta = std::max(0.0f, 1.0f - squared(cosTheta));
	float sinTheta = std::sqrt(sin2Theta);
	Vector localHalf(sinTheta * std::cos(phi),
		sinTheta * std::sin(phi),
		cosTheta);

	Vector x, y, z;
	makeCoordinateSpace(normal, x, y, z);
	Vector half = transformFromLocalSpace(localHalf, x, y, z);

	if (dot(outgoing, normal) < 0.0f)
		half *= -1.0f;

	outIncoming = outgoing - half * (2.0f * dot(outgoing, half));

	return evaluateSA(outIncoming, outgoing, normal, outPdf);
}

float Glossy::sampleSA(Vector& outIncoming, const Vector& outgoing, const Vector& normal, float u1, float u2, float& outPdf) const
{
	float phi = 2.0f * M_PI * u1;
	float cosTheta = std::pow(1.0f - u2, 1.0f / (exponent + 1.0f));
	float sin2Theta = std::max(0.0f, 1.0f - squared(cosTheta));
	float sinTheta = std::sqrt(sin2Theta);
	Vector localHalf(sinTheta * std::cos(phi),
		sinTheta * std::sin(phi),
		cosTheta);

	Vector x, y, z;
	makeCoordinateSpace(normal, x, y, z);
	Vector half = transformFromLocalSpace(localHalf, x, y, z);

	if (dot(outgoing, normal) < 0.0f)
		half *= -1.0f;

	outIncoming = outgoing - half * (2.0f * dot(outgoing, half));

	return evaluatePSA(outIncoming, outgoing, normal, outPdf);
}

float Glossy::pdfSA(const Vector& incoming, const Vector& outgoing, const Vector& normal) const
{
	float nDotI = dot(incoming, normal);
	float nDotO = dot(outgoing, normal);
	if ((nDotI > 0.0f && nDotO > 0.0f) ||
		(nDotI < 0.0f && nDotO < 0.0f))
	{
		return 0.0f;
	}

	Vector half;
	if (dot(outgoing, incoming) > 0.999f)
		half = normal;
	else
		half = (outgoing - incoming).normalized();

	return (exponent + 1.0f) * std::pow(std::fabs(dot(normal, half)), exponent) / (8.0f * M_PI * std::fabs(dot(outgoing, half)));
}

float Glossy::pdfPSA(const Vector& incoming, const Vector& outgoing, const Vector& normal) const
{
	float nDotI = dot(incoming, normal);
	float nDotO = dot(outgoing, normal);
	if ((nDotI > 0.0f && nDotO > 0.0f) ||
		(nDotI < 0.0f && nDotO < 0.0f))
	{
		return 0.0f;
	}

	Vector half;
	if (dot(outgoing, incoming) > 0.999f)
		half = normal;
	else
		half = (outgoing - incoming).normalized();

	return (exponent + 1.0f) * std::pow(std::fabs(dot(normal, half)), exponent) / (8.0f * M_PI * std::fabs(dot(outgoing, half))) * std::fabs(nDotI);
}

Color DiffuseMaterial::evaluate(
	const Point& position,
	const Vector& normal,
	const Vector& outgoingRayDirection,
	Brdf*& pBrdfChosen,
	float& brdfWeight)
{
	brdfWeight = 1.0f;
	pBrdfChosen = &lambert;
	return color;
}

Color GlossyMaterial::evaluate(
	const Point& position,
	const Vector& normal,
	const Vector& outgoingRayDirection,
	Brdf*& pBrdfChosen,
	float& brdfWeight)
{
	brdfWeight = 1.0f;
	pBrdfChosen = &glossy;
	return color;
}

Color Emitter::evaluate(
	const Point& position,
	const Vector& normal,
	const Vector& outgoingRayDirection,
	Brdf*& pBrdfChosen,
	float& brdfWeight)
{
	brdfWeight = 1.0f;
	pBrdfChosen = NULL;
	return Color();
}

Color Emitter::emittance()
{
	return color * power;
}