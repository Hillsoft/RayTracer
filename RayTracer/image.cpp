#include "image.h"
#include <fstream>
#include <iostream>

size_t Image::getWidth() const
{
	return width;
}

size_t Image::getHeight() const
{
	return height;
}

Color& Image::pixelXY(size_t x, size_t y, char wrapType)
{
	if (x < 0 || x >= width)
	{
		switch (wrapType)
		{
		case WRAP_CLAMP:
			if (x < 0)
				x = 0;
			else if (x >= width)
				x = width - 1;
			break;

		case WRAP_REPEAT:
			x = x % width;
			break;

		default:
		case WRAP_BLACK:
			pixels[width * height] = Color(0.0f);
			return pixels[width * height];
		}
	}

	if (y < 0 || y >= width)
	{
		switch (wrapType)
		{
		case WRAP_CLAMP:
			if (y < 0)
				y = 0;
			else if (y >= height)
				y = height - 1;
			break;

		case WRAP_REPEAT:
			y = y % height;
			break;

		default:
		case WRAP_BLACK:
			pixels[width * height] = Color(0.0f);
			return pixels[width * height];
		}
	}

	return pixels[y * width + x];
}

Color& Image::pixelUV(float u, float v, char wrapType)
{
	size_t x = (size_t)std::floor(u * (width - 1));
	size_t y = (size_t)std::floor(v * (height - 1));
	return pixelXY(x, y, wrapType);
}

#pragma pack(push, 1)
struct BitmapHeader
{
	char id1 = 'B';
	char id2 = 'M';
	int fileSize = 0; // Set
	short reserved1 = 0;
	short reserved2= 0;
	int startPos = 0x36;
	int subHeaderSize = 40;
	int width = 0; // Set
	int height = 0; // Set
	short colorplanes = 1;
	short bitsPerPixel = 24; // Set
	int compression = 0;
	int dataSize = 0; // Set
	int pixelPerMeterX = 0;
	int pixelPerMeterY = 0;
	int paletteSize = 0;
	int importantColors = 0;

	void write(std::ofstream& stream)
	{
		stream.write((char*)&id1, 1);
		stream.write((char*)&id2, 1);
		stream.write((char*)&fileSize, 4);
		stream.write((char*)&reserved1, 2);
		stream.write((char*)&reserved2, 2);
		stream.write((char*)&startPos, 4);
		stream.write((char*)&subHeaderSize, 4);
		stream.write((char*)&width, 4);
		stream.write((char*)&height, 4);
		stream.write((char*)&colorplanes, 2);
		stream.write((char*)&bitsPerPixel, 2);
		stream.write((char*)&compression, 4);
		stream.write((char*)&dataSize, 4);
		stream.write((char*)&pixelPerMeterX, 4);
		stream.write((char*)&pixelPerMeterY, 4);
		stream.write((char*)&paletteSize, 4);
		stream.write((char*)&importantColors, 4);
	}
};
#pragma pack(pop)

void Image::saveToFile(const char* filename) const
{
	const char* extension = strchr(filename, '.');
	
	if (!strcmp(extension, ".bmp"))
	{
		int padSize = 4 - ((3 * width) % 4);
		char pad[3];

		// Write BMP
		BitmapHeader head;
		head.bitsPerPixel = sizeof(char) * 3 * 8;
		head.dataSize = (head.bitsPerPixel / 8) * width * height + padSize * height;
		head.fileSize = head.dataSize + 54;
		head.width = (int)width;
		head.height = (int)height;

		std::ofstream outputFile(filename, std::ios::out | std::ios::binary);
		head.write(outputFile);

		// Create pixel data
		char curVals[3];
		for (int y = height - 1; y >= 0; y--)
		{
			for (int x = 0; x < width; x++)
			{
				Color curCol = pixels[y * width + x];
				curCol.clamp();
				curVals[0] = static_cast<unsigned char>(curCol.b * 255.0f);
				curVals[1] = static_cast<unsigned char>(curCol.g * 255.0f);
				curVals[2] = static_cast<unsigned char>(curCol.r * 255.0f);
				outputFile.write(curVals, 3);
			}
			outputFile.write(pad, padSize);
		}

		outputFile.flush();
		outputFile.close();
	}
}