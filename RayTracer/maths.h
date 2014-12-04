#ifndef __MATHS_H__
#define __MATHS_H__

#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979
#endif

inline float squared(float n) { return n * n; }


struct Color
{
	float r, g, b;

	Color() : r(0.0f), g(0.0f), b(0.0f) {}
	Color(const Color& c) : r(c.r), g(c.g), b(c.b) {}
	Color(float r, float g, float b) : r(r), g(g), b(b) {}
	explicit Color(float intensity) : r(intensity), g(intensity), b(intensity) {}

	inline void clamp(float min = 0.0f, float max = 0.0f)
	{
		r = std::max(std::min(r, max), min);
		g = std::max(std::min(g, max), min);
		b = std::max(std::min(b, max), min);
	}

	inline float brightness()
	{
		return r + g + b;
	}

	inline Color& operator =(const Color& c)
	{
		r = c.r;
		g = c.g;
		b = c.b;
		return *this;
	}

	inline Color& operator +=(const Color& c)
	{
		r += c.r;
		g += c.g;
		b += c.b;
		return *this;
	}

	inline Color& operator -=(const Color& c)
	{
		r -= c.r;
		g -= c.g;
		b -= c.b;
		return *this;
	}

	inline Color& operator *=(const Color& c)
	{
		r *= c.r;
		g *= c.g;
		b *= c.b;
		return *this;
	}

	inline Color& operator /=(const Color& c)
	{
		r /= c.r;
		g /= c.g;
		b /= c.b;
		return *this;
	}

	inline Color& operator *=(float f)
	{
		r *= f;
		g *= f;
		b *= f;
		return *this;
	}

	inline Color& operator /=(float f)
	{
		r /= f;
		g /= f;
		b /= f;
		return *this;
	}
};

inline Color operator +(const Color& c1, const Color& c2)
{
	return Color(c1.r + c2.r,
		c1.g + c2.g,
		c1.b + c2.b);
}

inline Color operator -(const Color& c1, const Color& c2)
{
	return Color(c1.r - c2.r,
		c1.g - c2.g,
		c1.b - c2.b);
}

inline Color operator *(const Color& c1, const Color& c2)
{
	return Color(c1.r * c2.r,
		c1.g * c2.g,
		c1.b * c2.b);
}

inline Color operator /(const Color& c1, const Color& c2)
{
	return Color(c1.r / c2.r,
		c1.g / c2.g,
		c1.b / c2.b);
}

inline Color operator *(const Color& c, float f)
{
	return Color(c.r * f,
		c.g * f,
		c.b * f);
}

inline Color operator *(float f, const Color& c)
{
	return Color(c.r * f,
		c.g * f,
		c.b * f);
}

inline Color operator /(const Color& c, float f)
{
	return Color(c.r / f,
		c.g / f,
		c.b / f);
}


struct Vector
{
	float x, y, z;

	Vector() : x(0.0f), y(0.0f), z(0.0f) {}
	Vector(const Vector& v) : x(v.x), y(v.y), z(v.z) {}
	Vector(float x, float y, float z) : x(x), y(y), z(z) {}
	explicit Vector(float f) : x(f), y(f), z(f) {}

	inline float length2() const
	{
		return squared(x) + squared(y) + squared(z);
	}

	inline float length() const
	{
		return std::sqrt(length2());
	}

	float normalize()
	{
		float len = length();
		*this /= len;
		return len;
	}
	
	Vector normalized() const
	{
		Vector nVec(*this);
		nVec.normalize();
		return nVec;
	}

	inline Vector& operator =(const Vector& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}

	inline Vector& operator +=(const Vector& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	inline Vector& operator -=(const Vector& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	inline Vector& operator *=(float f)
	{
		x *= f;
		y *= f;
		z *= f;
		return *this;
	}

	inline Vector& operator /=(float f)
	{
		x /= f;
		y /= f;
		z /= f;
		return *this;
	}

	inline Vector operator -() const
	{
		return Vector(-x, -y, -z);
	}
};

inline Vector operator +(const Vector& v1, const Vector& v2)
{
	return Vector(v1.x + v2.x,
		v1.y + v2.y,
		v1.z + v2.z);
}

inline Vector operator -(const Vector& v1, const Vector& v2)
{
	return Vector(v1.x - v2.x,
		v1.y - v2.y,
		v1.z - v2.z);
}

inline Vector operator *(const Vector& v, float f)
{
	return Vector(v.x * f,
		v.y * f,
		v.z * f);
}

inline Vector operator *(float f, const Vector& v)
{
	return Vector(v.x * f,
		v.y * f,
		v.z * f);
}

inline Vector operator /(const Vector& v, float f)
{
	return Vector(v.x / f,
		v.y / f,
		v.z / f);
}

inline float dot(const Vector& v1, const Vector& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

inline Vector cross(const Vector& v1, const Vector& v2)
{
	return Vector(v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x);
}

typedef Vector Point;

#endif