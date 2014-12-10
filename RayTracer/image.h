#ifndef __IMAGE_H__
#define __IMAGE_H__

#define WRAP_BLACK 0
#define WRAP_CLAMP 1
#define WRAP_REPEAT 2

#include "maths.h"

class Image
{
public:
	Image(size_t width, size_t height)
		: width(width), height(height),
		pixels(new Color[width * height + 1]) { }

	virtual ~Image() { delete[] pixels; }

	size_t getWidth() const;
	size_t getHeight() const;

	Color& pixelXY(size_t x, size_t y, char wrapType = WRAP_BLACK);
	Color& pixelUV(float u, float v, char wrapType = WRAP_BLACK);

	void saveToFile(const char* filename) const;

protected:
	size_t width, height;
	Color *pixels;
};

#endif